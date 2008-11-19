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

#include "platform.hpp"
#ifdef ZMQ_HAVE_SOLARIS

#include <stdlib.h>
#include <sys/devpoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "err.hpp"
#include "devpoll_thread.hpp"

using namespace zmq;

i_thread *devpoll_thread_t::create (dispatcher_t *dispatcher_)
{
    return new devpoll_thread_t (dispatcher_);
}

devpoll_thread_t::devpoll_thread_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    struct rlimit rl;

    //  Get limit on open files
    int rc = getrlimit (RLIMIT_NOFILE, &rl);
    errno_assert (rc != -1);

    poll_table = (struct poll_entry**)
        calloc (maxfds, sizeof (struct poll_entry*));
    assert (poll_table != NULL);

    devpoll_fd = open ("/dev/poll", O_RDWR);
    errno_assert (devpoll_fd != -1);

    //  Add signaller into out interest set.
    devpoll_write (signaler.get_fd (), POLLIN);

    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (&signaler);

    //  Create the worker thread.
    worker = new thread_t (worker_routine, this);
}

devpoll_thread_t::~devpoll_thread_t ()
{
    //  Send a 'stop' event ot the worker thread.
    //  TODO: Analyse whether using the 'to-self' command pipe here
    //        is appropriate.
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);

    //  Wait till the worker thread terminates.
    delete worker;

    dispatcher->deallocate_thread_id (thread_id);
    close (devpoll_fd);
    free (poll_table);
}

void devpoll_thread_t::devpoll_write (int fd_, short events_)
{
    struct pollfd pfd;

    pfd.fd = fd_;
    pfd.events = events_;

    ssize_t rc = write (devpoll_fd, &pfd, sizeof pfd);
    errno_assert (rc == sizeof pfd);
}

int devpoll_thread_t::get_thread_id ()
{
    return thread_id;
}

void devpoll_thread_t::send_command (i_thread *destination_,
    const command_t &command_)
{
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
        dispatcher->write (thread_id, destination_->get_thread_id (), command_);
}

devpoll_thread_t::poll_entry *
devpoll_thread_t::new_poll_entry (int fd_, i_pollable *engine_)
{
    struct poll_entry *pe;

    pe = (struct poll_entry*) malloc (sizeof (struct poll_entry));
    if (pe) {
        pe->fd = fd_;
        pe->events = 0;
        pe->engine = engine_;
    }

    return pe;
}

handle_t devpoll_thread_t::add_fd (int fd_, i_pollable *engine_)
{
    assert (fd_ < maxfds);

    struct poll_entry *pe = new_poll_entry (fd_, engine_);
    assert (pe != NULL);
    poll_table [fd_] = pe;

    devpoll_write (fd_, 0);

    object_table_t::iterator it = engines.find (engine_);
    if (it == engines.end ())
        engines.insert (object_table_t::value_type (engine_, 1));
    else
        it->second++;

    handle_t handle;
    handle.ptr = pe;
    return handle;
}

void devpoll_thread_t::rm_fd (handle_t handle_)
{
    struct poll_entry *pe = (struct poll_entry*) handle_.ptr;

    object_table_t::iterator it = engines.find (pe->engine);
    assert (it != engines.end ());

    if (--it->second == 0)
        engines.erase (it);

    devpoll_write (pe->fd, POLLREMOVE);
    free (pe);
}

void devpoll_thread_t::set_pollin (handle_t handle_)
{
    struct poll_entry *pe = (struct poll_entry*) handle_.ptr;

    if ((pe->events & POLLIN) == 0) {
        pe->events |= POLLIN;
        devpoll_write (pe->fd, pe->events);
    }
}

void devpoll_thread_t::reset_pollin (handle_t handle_)
{
    struct poll_entry *pe = (struct poll_entry*) handle_.ptr;

    if ((pe->events & POLLIN) == POLLIN) {
        pe->events &= ~((short) POLLIN);
        devpoll_write (pe->fd, pe->events);
    }
}

void devpoll_thread_t::set_pollout (handle_t handle_)
{
    struct poll_entry *pe = (struct poll_entry*) handle_.ptr;

    if ((pe->events & POLLOUT) == 0) {
        pe->events |= POLLOUT;
        devpoll_write (pe->fd, pe->events);
    }
}

void devpoll_thread_t::reset_pollout (handle_t handle_)
{
    struct poll_entry *pe = (struct poll_entry*) handle_.ptr;

    if ((pe->events & POLLOUT) == POLLOUT) {
        pe->events &= ~((short) POLLOUT);
        devpoll_write (pe->fd, pe->events);
    }
}

void devpoll_thread_t::worker_routine (void *arg_)
{
    devpoll_thread_t *self = (devpoll_thread_t*) arg_;
    self->loop ();
}

void devpoll_thread_t::loop ()
{
    int nfds = epoll_max_events;
    if (nfds >= maxfds)
        nfds = maxfds - 1;

    while (true) {
        struct pollfd poll_buf [epoll_max_events];
        struct dvpoll poll_req;

        poll_req.dp_fds = &poll_buf [0];
        poll_req.dp_nfds = nfds;
        poll_req.dp_timeout = -1;

        //  Wait for events.
        int n = ioctl (devpoll_fd, DP_POLL, &poll_req);
        errno_assert (n != -1);

        for (struct pollfd *pfd = &poll_buf [0]; pfd < &poll_buf [n]; pfd++) {
            if (pfd->fd == signaler.get_fd ()) {
                if (pfd->revents & POLLIN) {
                    uint32_t signals = signaler.check ();
                    assert (signals);
                    if (!process_commands (signals))
                        return;
                }
            }
            else {
                struct poll_entry *pe = poll_table [pfd->fd];
                assert (pe != NULL);

                //  Process out events from the engine.
                if (pfd->revents & (POLLERR | POLLHUP))
                    pe->engine->error_event ();

                //  Process out events from the engine.
                if (pfd->revents & POLLOUT)
                    pe->engine->out_event ();

                //  Process in events from the engine.
                if (pfd->revents & POLLIN)
                    pe->engine->in_event ();
            }
        }
    }
}

bool zmq::devpoll_thread_t::process_command (const command_t &command_)
{
    i_engine *engine;

    switch (command_.type) {

    //  Exit the working thread.
    case command_t::stop:
        return false;

    //  Register the engine supplied with the poll thread.
    case command_t::register_engine:

        //  Ask engine to register itself.
        engine = command_.args.register_engine.engine;
        assert (engine->type () == engine_type_fd);
        ((i_pollable*) engine)->register_event (this);
        return true;

    //  Unregister the engine.
    case command_t::unregister_engine:

        //  Ask engine to unregister itself.
        engine = command_.args.unregister_engine.engine;
        assert (engine->type () == engine_type_fd);
        ((i_pollable*) engine)->unregister_event ();
        return true;

    //  Forward the command to the specified engine.
    case command_t::engine_command:

        //  Forward the command to the engine.
        engine = command_.args.register_engine.engine;
        engine->process_command (
            command_.args.engine_command.command);
        return true;

    //  Unknown command.
    default:
        assert (false);
        return false;
    }
}

bool devpoll_thread_t::process_commands (uint32_t signals_)
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
