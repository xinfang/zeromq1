/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <zmq/io_thread.hpp>
#include <zmq/select.hpp>
#include <zmq/poll.hpp>
#include <zmq/epoll.hpp>
#include <zmq/devpoll.hpp>
#include <zmq/kqueue.hpp>
#include <zmq/err.hpp>

zmq::i_thread *zmq::io_thread_t::create (dispatcher_t *dispatcher_)
{
    //  Create the object.
    io_thread_t *thread = new io_thread_t (dispatcher_);
    zmq_assert (thread);

    return thread;
}

void zmq::io_thread_t::destroy ()
{
    //  Wait for termination of the underlying I/O thread.
    poller->terminate_shutdown ();

    //  TODO: At this point terminal handshaking should be done.
    //  Afterwards 'delete this' can be executed. 
}

zmq::io_thread_t::io_thread_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    //  TODO: It would be nice to allow user to specify what kind of
    //  polling mechanism to use.

#if defined (ZMQ_HAVE_LINUX)
    poller = new epoll_t;
#elif defined (ZMQ_HAVE_WINDOWS)
    poller = new select_t;
#elif defined (ZMQ_HAVE_FREEBSD)
    poller = new kqueue_t;
#elif defined(ZMQ_HAVE_OPENBSD)
    poller = new kqueue_t;
#elif defined (ZMQ_HAVE_SOLARIS)
    poller = new devpoll_t;
#elif defined (ZMQ_HAVE_OSX)
    poller = new kqueue_t;
#elif defined (ZMQ_HAVE_QNXNTO)
    poller = new poll_t;
#elif defined (ZMQ_HAVE_AIX)
    poller = new poll_t;
#elif defined (ZMQ_HAVE_HPUX)
    poller = new devpoll_t;
#elif defined (ZMQ_HAVE_OPENVMS)
    poller = new select_t;
#else
#error "Unsupported platform"
#endif
    zmq_assert (poller);

    signaler_handle = poller->add_fd (signaler.get_fd (), this);
    poller->set_pollin (signaler_handle);

    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (this, &signaler);

    //  Start the underlying I/O thread.
    poller->start ();
}

zmq::io_thread_t::~io_thread_t ()
{
    poller->rm_fd (signaler_handle);
    delete poller;
}

int zmq::io_thread_t::get_thread_id ()
{
    return thread_id;
}

void zmq::io_thread_t::send_command (i_thread *destination_,
    const command_t &command_)
{
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
        dispatcher->write (thread_id,
            destination_->get_thread_id (), command_);
}

void zmq::io_thread_t::stop ()
{
    //  'to-self' command pipe is used solely for the 'stop' command.
    //  This way there's no danger of 2 threads accessing the pipe
    //  at the same time.
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);
}

void zmq::io_thread_t::process_command (const command_t &command_)
{
    i_engine *engine;
    pipe_t *pipe;

    switch (command_.type) {

    case command_t::stop:
        {
            //  Unregister all the registered engines.
            for (engines_t::iterator it = engines.begin ();
                  it != engines.end (); it ++)
                (*it)->cast_to_pollable ()->unregister_event ();

            //  Start shutdown process.
            poller->initialise_shutdown ();
        }
        break;

    case command_t::revive_reader:
        pipe = command_.args.revive_reader.pipe;
        pipe->revive_reader ();
        break;

    case command_t::notify_writer:
        pipe = command_.args.notify_writer.pipe;
        pipe->notify_writer (command_.args.notify_writer.position);
        break;

    case command_t::terminate_pipe_req:
        pipe = command_.args.terminate_pipe_req.pipe;
        pipe->terminate_pipe_req ();
        break;

    case command_t::terminate_pipe_ack:
        pipe = command_.args.terminate_pipe_ack.pipe;
        pipe->terminate_pipe_ack ();
        break;

    //  Register the engine supplied with the poll thread.
    case command_t::register_engine:

        //  Ask engine to register itself.
        engine = command_.args.register_engine.engine;
        engine->cast_to_pollable ()->register_event (poller);
        engines.push_back (engine);
        break;

    //  Unregister the engine.
    case command_t::unregister_engine:

        //  Ask engine to unregister itself.
        engine = command_.args.unregister_engine.engine;
        engine->cast_to_pollable ()->unregister_event ();
        break;

    //  Forward the command to the specified engine.
    case command_t::engine_command:
        {
            //  Forward the command to the engine.
            engine = command_.args.engine_command.engine;
            const engine_command_t &engcmd =
                command_.args.engine_command.command;
            switch (engcmd.type) {
            case engine_command_t::send_to:
                engine->send_to (engcmd.args.send_to.pipe);
                break;
            case engine_command_t::receive_from:
                engine->receive_from (engcmd.args.receive_from.pipe);
                break;
            default:

                //  Unknown engine command.
                zmq_assert (false);
            }
            break;
        }

    //  Unknown command.
    default:
        zmq_assert (false);
    }
}

void zmq::io_thread_t::register_event (i_poller*)
{
    //  This is not an engine. This function is never called.
    zmq_assert (false);
}

void zmq::io_thread_t::in_event ()
{
    //  Find out which threads are sending us commands.
    uint32_t signals = signaler.check ();
    zmq_assert (signals);

    //  Iterate through all the threads in the process and find out
    //  which of them sent us commands.
    for (int source_thread_id = 0;
          source_thread_id != dispatcher->get_thread_count ();
          source_thread_id ++) {
        if (signals & (1 << source_thread_id)) {

            //  Read all the commands from particular thread.
            command_t command;
            while (dispatcher->read (source_thread_id, thread_id, &command))
                process_command (command);
        }
    }
}

void zmq::io_thread_t::out_event ()
{
    //  We are never polling for POLLOUT here. This function is never called.
    zmq_assert (false);
}

void zmq::io_thread_t::timer_event ()
{
    //  No timers here. This function is never called.
    zmq_assert (false);
}

void zmq::io_thread_t::unregister_event ()
{
    //  This is not an engine. This function is never called.
    zmq_assert (false);

}
