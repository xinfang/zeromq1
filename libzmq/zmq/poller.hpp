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

#ifndef __ZMQ_POLLER_HPP_INCLUDED__
#define __ZMQ_POLLER_HPP_INCLUDED__

#include <vector>
#include <cstdlib>
#include <algorithm>

#include <zmq/i_thread.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/ysocketpair.hpp>
#include <zmq/thread.hpp>
#include <zmq/fd.hpp>

namespace zmq
{

    template <class T> class poller_t : public i_thread, public i_pollable
    {
    public:

        static i_thread *create (dispatcher_t *dispatcher_);
        
        //  i_thread implementation.
        int get_thread_id ();
        void send_command (i_thread *destination_, const command_t &command_);
        void stop ();
        void destroy ();

        //  i_pollable implementation.
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void timer_event ();
        void unregister_event ();

    private:

        poller_t (dispatcher_t *dispatcher_);
        ~poller_t ();

        //  Processes individual command.
        void process_command (const command_t &command_);

        //  Pointer to dispatcher.
        dispatcher_t *dispatcher;

        //  Thread ID allocated for the poll thread by dispatcher.
        int thread_id;

        //  Poll thread gets notifications about incoming commands using
        //  this socketpair.
        ysocketpair_t signaler;

        //  Handle associated with signaler's file descriptor.
        handle_t signaler_handle;

        //  We perform I/O multiplexing using event monitor.
        T *event_monitor;

        //  List of all registered engines.
        typedef std::vector <i_engine*> engines_t;
        engines_t engines;
    };

}

template <class T>
zmq::i_thread *zmq::poller_t <T>::create (dispatcher_t *dispatcher_)
{
    //  Create the object.
    poller_t <T> *poller = new poller_t <T> (dispatcher_);
    zmq_assert (poller);

    return poller;
}

template <class T>
void zmq::poller_t <T>::destroy ()
{
    //  Wait for termination of the underlying I/O thread.
    event_monitor->terminate_shutdown ();

    //  TODO: At this point terminal handshaking should be done.
    //  Afterwards 'delete this' can be executed. 
}

template <class T>
zmq::poller_t <T>::poller_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    event_monitor = new T;
    zmq_assert (event_monitor);

    signaler_handle = event_monitor->add_fd (signaler.get_fd (), this);
    event_monitor->set_pollin (signaler_handle);

    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (this, &signaler);

    //  Start the underlying I/O thread.
    event_monitor->start ();
}

template <class T>
zmq::poller_t <T>::~poller_t ()
{
    event_monitor->rm_fd (signaler_handle);

    delete event_monitor;
}

template <class T>
int zmq::poller_t <T>::get_thread_id ()
{
    return thread_id;
}

template <class T>
void zmq::poller_t <T>::send_command (i_thread *destination_,
    const command_t &command_)
{
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
        dispatcher->write (thread_id,
            destination_->get_thread_id (), command_);
}

template <class T>
void zmq::poller_t <T>::stop ()
{
    //  'to-self' command pipe is used solely for the 'stop' command.
    //  This way there's no danger of 2 threads accessing the pipe
    //  at the same time.
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);
}

template <class T>
void zmq::poller_t <T>::process_command (const command_t &command_)
{
    i_engine *engine;
    pipe_t *pipe;

    switch (command_.type) {

    //  Exit the working thread.
    case command_t::stop:
        {
            //  Unregister all the registered engines.
            for (engines_t::iterator it = engines.begin ();
                  it != engines.end (); it ++)
                (*it)->cast_to_pollable ()->unregister_event ();

            event_monitor->initialise_shutdown ();
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
        engine->cast_to_pollable ()->register_event (event_monitor);
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

template <class T>
void zmq::poller_t <T>::register_event (i_poller*)
{
    //  This is not an engine. This function is never called.
    zmq_assert (false);
}

template <class T>
void zmq::poller_t <T>::in_event ()
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

template <class T>
void zmq::poller_t <T>::out_event ()
{
    //  We are never polling for POLLOUT here. This function is never called.
    zmq_assert (false);
}

template <class T>
void zmq::poller_t <T>::timer_event ()
{
    //  No timers here. This function is never called.
    zmq_assert (false);
}

template <class T>
void zmq::poller_t <T>::unregister_event ()
{
    //  This is not an engine. This function is never called.
    zmq_assert (false);

}

#endif
