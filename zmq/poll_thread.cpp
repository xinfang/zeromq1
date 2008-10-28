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
#if defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_FREEBSD ||\
    defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_SOLARIS
#include <sys/resource.h>
#endif

#include "poll_thread.hpp"
#include "err.hpp"

#if defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_FREEBSD ||\
    defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_SOLARIS

zmq::i_thread *zmq::poll_thread_t::create (dispatcher_t *dispatcher_)
{
    return new poll_thread_t (dispatcher_);
}

zmq::poll_thread_t::poll_thread_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_),
    pollset (1)
{
    //  Get limit on open file descriptors. Resize fds to make it able to
    //  hold all the descriptors.
    rlimit rl;
    int rc = getrlimit (RLIMIT_NOFILE, &rl);
    errno_assert (rc != -1);
    fds.resize (rl.rlim_cur, -1);

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

zmq::handle_t zmq::poll_thread_t::add_fd (int fd_, i_pollable *engine_)
{
    pollfd pfd = {fd_, 0, 0};
    pollset.push_back (pfd);
    engines.push_back (engine_);
    assert (fds [fd_] == -1);
    fds [fd_] = pollset.size() - 1;

    handle_t handle;
    handle.fd = fd_;
    return handle;
}

void zmq::poll_thread_t::rm_fd (handle_t handle_)
{
    //  Remove the descriptor from pollset and engine list.
    int index = fds [handle_.fd];
    assert (index != -1);
    pollset.erase (pollset.begin () + index);
    engines.erase (engines.begin () + index - 1);

    //  Mark the fd as unused.
    fds [handle_.fd] = -1;

    //  Adjust fd list to match new indices to the pollset. To make it more
    //  efficient we are traversing the pollset whitch is shorter than
    //  fd list itself.
    for (int i = index; i != (int) pollset.size (); i++)
        fds [pollset [i].fd] = i;    
}

void zmq::poll_thread_t::set_pollin (handle_t handle_)
{
    pollset [fds [handle_.fd]].events |= POLLIN;
}

void zmq::poll_thread_t::reset_pollin (handle_t handle_)
{
    pollset [fds [handle_.fd]].events &= ~((short) POLLIN);
}

void zmq::poll_thread_t::speculative_read (handle_t handle_)
{
    pollset [fds [handle_.fd]].events |= POLLIN;
    pollset [fds [handle_.fd]].revents |= POLLIN;
}

void zmq::poll_thread_t::set_pollout (handle_t handle_)
{
    pollset [fds [handle_.fd]].events |= POLLOUT;
}

void zmq::poll_thread_t::reset_pollout (handle_t handle_)
{
    pollset [fds [handle_.fd]].events &= ~((short) POLLOUT);
}

void zmq::poll_thread_t::speculative_write (handle_t handle_)
{
    pollset [fds [handle_.fd]].events |= POLLOUT;
    pollset [fds [handle_.fd]].revents |= POLLOUT;
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
                        i_engine *engine =
                            command.args.register_engine.engine;
                        assert (engine->type () == engine_type_fd);
                        ((i_pollable*) engine)->register_event (this);
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
                        i_engine *engine =
                            command.args.unregister_engine.engine;
                        assert (engine->type () == engine_type_fd);
                        ((i_pollable*) engine)->unregister_event ();
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
