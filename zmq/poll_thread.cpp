/*
    Copyright (c) 2007-2008 FastMQ Inc.

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

#include <algorithm>

#include "poll_thread.hpp"
#include "err.hpp"

#if defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_FREEBSD || defined ZMQ_HAVE_OSX

zmq::i_thread *zmq::poll_thread_t::create (dispatcher_t *dispatcher_)
{
    return new poll_thread_t (dispatcher_);
}

zmq::poll_thread_t::poll_thread_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_),
    pollset (1)
{
    //  Initialise the pollset.
    pollset [0].fd = signaler.get_fd ();
    pollset [0].events = POLLIN;

    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (&signaler);

    //  Create the worker thread.
    worker = new thread_t (worker_routine, this);
}

zmq::poll_thread_t::~poll_thread_t ()
{
    //  Send a 'stop' event ot the worker thread.
    //  TODO: Analyse whether using the 'to-self' command pipe here
    //        is appropriate.
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);

    //  Wait till worker thread terminates.
    delete worker;
}

int zmq::poll_thread_t::get_thread_id ()
{
    return thread_id;
}

void zmq::poll_thread_t::send_command (i_thread *destination_,
    const command_t &command_)
{
    dispatcher->write (thread_id, destination_->get_thread_id (), command_);
}

int zmq::poll_thread_t::add_fd (int fd_, i_pollable *engine_)
{
    pollfd pfd = {fd_, 0, 0};
    pollset.push_back (pfd);
    engines.push_back (engine_);
    return pollset.size () - 1;
}

void zmq::poll_thread_t::rm_fd (int handle_)
{
    assert (false);
}

void zmq::poll_thread_t::set_pollin (int handle_)
{
    pollset [handle_].events |= POLLIN;
}

void zmq::poll_thread_t::reset_pollin (int handle_)
{
    pollset [handle_].events &= ~((short) POLLIN);
}

void zmq::poll_thread_t::speculative_read (int handle_)
{
    pollset [handle_].events |= POLLIN;
    pollset [handle_].revents |= POLLIN;
}

void zmq::poll_thread_t::set_pollout (int handle_)
{
    pollset [handle_].events |= POLLOUT;
}

void zmq::poll_thread_t::reset_pollout (int handle_)
{
    pollset [handle_].events &= ~((short) POLLOUT);
}

void zmq::poll_thread_t::speculative_write (int handle_)
{
    pollset [handle_].events |= POLLOUT;
    pollset [handle_].revents |= POLLOUT;
}

void zmq::poll_thread_t::worker_routine (void *arg_)
{
    poll_thread_t *self = (poll_thread_t*) arg_;
    self->loop ();
}

void zmq::poll_thread_t::loop ()
{
    while (true)
    {
        //  Wait for events.
        int rc = poll (&pollset [0], pollset.size (), -1);
        errno_assert (rc != -1);

        //  First of all, process commands from other threads.
        if (pollset [0].revents & POLLIN) {
            uint32_t signals = signaler.check ();
            assert (signals);
            if (!process_commands (signals))
                return;
        }

        //  Process socket errors.
        for (pollset_t::size_type pollset_index = 1;
              pollset_index != pollset.size (); pollset_index ++)
            if (pollset [pollset_index].revents &
                  (POLLNVAL | POLLERR | POLLHUP))
                engines [pollset_index - 1]->error_event ();

        //  Process out events from the engines.
        for (pollset_t::size_type pollset_index = 1;
              pollset_index != pollset.size (); pollset_index ++)
            if (pollset [pollset_index].revents & POLLOUT)
                engines [pollset_index - 1]->out_event ();

        //  Process in events from the engines.
        //  TODO: investigate the POLLHUP issue on OS X
        for (pollset_t::size_type pollset_index = 1;
              pollset_index != pollset.size (); pollset_index ++)
            if (pollset [pollset_index].revents & POLLIN)
                engines [pollset_index - 1]->in_event ();
    }
}

bool zmq::poll_thread_t::process_commands (uint32_t signals_)
{
    //  Iterate through all the threads in the process and find out which
    //  of them sent us commands.
    for (int source_thread_id = 0;
          source_thread_id != dispatcher->get_thread_count ();
          source_thread_id ++) {
        if (signals_ & (1 << source_thread_id)) {

            //  Read all the commands from particular thread.
            command_t command;
            while (dispatcher->read (source_thread_id, thread_id, &command)) {

                switch (command.type) {

                //  Exit the working thread.
                case command_t::stop:
                    return false;

                //  Register the engine supplied with the poll thread.
                case command_t::register_engine:
                    {
                        //  Ask engine to register itself.
                        i_pollable *engine =
                            command.args.register_engine.engine;
                        engine->register_event (this);
                    }
                    break;

                //  Unregister the engine.
                case command_t::unregister_engine:
                    {
                        //  Assert that engine still exists.
                        //  TODO: We should somehow make sure this won't happen.
                        std::vector <i_pollable*>::iterator it = std::find (
                            engines.begin (), engines.end (),
                            command.args.unregister_engine.engine);
                        assert (it != engines.end ());

                        //  Ask engine to unregister itself.
                        i_pollable *engine =
                            command.args.register_engine.engine;
                        engine->unregister_event ();
                    }
                    break;


                //  Forward the command to the specified engine.
                case command_t::engine_command:
                    {
                        //  Check whether engine still exists.
                        //  TODO: We should somehow make sure this won't happen.
                        std::vector <i_pollable*>::iterator it = std::find (
                            engines.begin (), engines.end (),
                            command.args.engine_command.engine);

                        //  Forward the command to the engine.
                        //  TODO: If the engine doesn't exist drop the command.
                        //        However, imagine there's another engine
                        //        incidentally allocated on the same address.
                        if (it != engines.end ())
                            command.args.engine_command.engine->process_command(
                                command.args.engine_command.command);
                    }
                    break;

                //  Unknown command.
                default:
                    assert (false);
                }
            }
        }
    }
    return true;
}

#endif
