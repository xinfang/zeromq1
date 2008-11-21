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

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_FREEBSD ||\
    defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_SOLARIS ||\
    defined ZMQ_HAVE_OPENBSD || defined ZMQ_HAVE_QNXNTO

#include <algorithm>
#include <sys/resource.h>

#include <zmq/poll_thread.hpp>
#include <zmq/err.hpp>

zmq::i_thread *zmq::poll_thread_t::create (dispatcher_t *dispatcher_)
{
    return new poll_thread_t (dispatcher_);
}

zmq::poll_thread_t::poll_thread_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_),
    pollset (1),
    removed_fds (false)
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
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
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

    //  Mark fd for deletion, eg. setting fd to -1 and revents 0.
    pollset [index].fd = -1;
    pollset [index].revents = 0;

    //  We have fd scheduled for deletion.
    removed_fds = true;

    //  Mark the fd as unused.
    fds [handle_.fd] = -1;
}

void zmq::poll_thread_t::set_pollin (handle_t handle_)
{
    pollset [fds [handle_.fd]].events |= POLLIN;
}

void zmq::poll_thread_t::reset_pollin (handle_t handle_)
{
    pollset [fds [handle_.fd]].events &= ~((short) POLLIN);
}

void zmq::poll_thread_t::set_pollout (handle_t handle_)
{
    pollset [fds [handle_.fd]].events |= POLLOUT;
}

void zmq::poll_thread_t::reset_pollout (handle_t handle_)
{
    pollset [fds [handle_.fd]].events &= ~((short) POLLOUT);
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

        //  Process out events from the engines.
        for (pollset_t::size_type pollset_index = 1;
              pollset_index < pollset.size (); pollset_index ++)
            if (pollset [pollset_index].revents & POLLOUT)
                engines [pollset_index - 1]->out_event ();


        //  Process the rest of the socket events.
        for (pollset_t::size_type pollset_index = 1;
              pollset_index < pollset.size (); pollset_index ++) {

            //  Invalid fd in poll
            assert (!(pollset [pollset_index].revents & POLLNVAL));

            if (pollset [pollset_index].revents &
                  (POLLIN | POLLERR | POLLHUP)) {
                //  Note that error is handled  by the
                //  in_event in the case of error reading from the socket.
                engines [pollset_index - 1]->in_event ();
            }
        }

        //  Check if we have some fd to delete from pollset.
        if (removed_fds) {
            for (pollset_t::size_type pollset_index = 1; 
                  pollset_index < pollset.size (); pollset_index ++) {
                if (pollset [pollset_index].fd == -1) {
                    pollset.erase (pollset.begin () + pollset_index);
                    engines.erase (engines.begin () + pollset_index - 1);

                    //  Adjust fd list to match new indices to the pollset.
                    //  To make it more efficient we are traversing the pollset
                    //  whitch is shorter than fd list itself.
                    for (pollset_t::size_type i = pollset_index;
                          i != pollset.size (); i ++)
                        fds [pollset [i].fd] = i; 

                    //  Adjust index to the pollset to compensate for the
                    //  file descriptor removal.
                    pollset_index --;
                }
            }

            removed_fds = false;
        }
    }
}

bool zmq::poll_thread_t::process_command (const command_t &command_)
{
    switch (command_.type) {

    //  Exit the working thread.
    case command_t::stop:
        return false;

    //  Register the engine supplied with the poll thread.
    case command_t::register_engine:
        {
            //  Ask engine to register itself.
            i_engine *engine = command_.args.register_engine.engine;
            assert (engine->type () == engine_type_fd);
            ((i_pollable*) engine)->register_event (this);
        }
        return true;

    //  Unregister the engine.
    case command_t::unregister_engine:
        {
            //  Ask engine to unregister itself.
            i_engine *engine = command_.args.unregister_engine.engine;
            assert (engine->type () == engine_type_fd);
            ((i_pollable*) engine)->unregister_event ();
        }
        return true;

    //  Forward the command to the specified engine.
    case command_t::engine_command:

        //  Forward the command to the engine.
        command_.args.engine_command.engine->process_command (
            command_.args.engine_command.command);
        return true;

    //  Unknown command.
    default:
        assert (false);
        return false;
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
            while (dispatcher->read (source_thread_id, thread_id, &command))
                if (!process_command (command))
                    return false;
        }
    }
    return true;
}

#endif
