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
#ifdef ZMQ_HAVE_LINUX

#include "epoll_thread.hpp"
#include "err.hpp"

using namespace zmq;

i_thread *epoll_thread_t::create (dispatcher_t *dispatcher_)
{
    return new epoll_thread_t (dispatcher_);
}

epoll_thread_t::epoll_thread_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    struct epoll_event ev;

    epfd = epoll_create (1);
    errno_assert (epfd != -1);

    //  Initialize epoll event structure for command signaller.
    ev.events = EPOLLIN;
    ev.data.ptr = NULL;

    //  Add command signaller into epoll set.
    int rc = epoll_ctl (epfd, EPOLL_CTL_ADD, signaler.get_fd (), &ev);
    errno_assert (rc != -1);

    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (&signaler);

    //  Create the worker thread.
    worker = new thread_t (worker_routine, this);
}

epoll_thread_t::~epoll_thread_t ()
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
    close (epfd);
}

int epoll_thread_t::get_thread_id ()
{
    return thread_id;
}

void epoll_thread_t::send_command (i_thread *destination_,
    const command_t &command_)
{
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
        dispatcher->write (thread_id, destination_->get_thread_id (), command_);
}

handle_t epoll_thread_t::add_fd (int fd_, i_pollable *engine_)
{
    poll_entry *pe = (poll_entry*) malloc (sizeof (poll_entry));
    errno_assert (pe);
    pe->ev.events = 0;
    pe->ev.data.ptr = pe;
    pe->fd = fd_;
    pe->engine = engine_;

    int rc = epoll_ctl (epfd, EPOLL_CTL_ADD, fd_, &pe->ev);
    errno_assert (rc != -1);

    object_table_t::iterator it = engines.find (engine_);
    if (it == engines.end ())
        engines.insert (object_table_t::value_type (engine_, 1));
    else
        it->second++;

    handle_t handle;
    handle.ptr = pe;
    return handle;
}

void epoll_thread_t::rm_fd (handle_t handle_)
{
    poll_entry *pe = (poll_entry*) handle_.ptr;

    object_table_t::iterator it = engines.find (pe->engine);
    assert (it != engines.end ());

    if (--it->second == 0)
        engines.erase (it);

    int rc = epoll_ctl (epfd, EPOLL_CTL_DEL, pe->fd, &pe->ev);
    errno_assert (rc != -1);

    free (pe);
}

void epoll_thread_t::set_pollin (handle_t handle_)
{
    poll_entry *pe = (poll_entry*) handle_.ptr;
    pe->ev.events |= EPOLLIN;
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void epoll_thread_t::reset_pollin (handle_t handle_)
{
    poll_entry *pe = (poll_entry*) handle_.ptr;
    pe->ev.events &= ~((short) EPOLLIN);
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void epoll_thread_t::set_pollout (handle_t handle_)
{
    struct poll_entry *poll_entry = (struct poll_entry*) handle_.ptr;
    poll_entry->ev.events |= EPOLLOUT;
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, poll_entry->fd, &poll_entry->ev);
    errno_assert (rc != -1);
}

void epoll_thread_t::reset_pollout (handle_t handle_)
{
    poll_entry *pe = (poll_entry*) handle_.ptr;
    pe->ev.events &= ~((short) EPOLLOUT);
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void epoll_thread_t::worker_routine (void *arg_)
{
    epoll_thread_t *self = (epoll_thread_t*) arg_;
    self->loop ();
}

void epoll_thread_t::loop ()
{
    struct epoll_event events [epoll_max_events];

    while (true) {

        //  Wait for events.
        int rc = epoll_wait (epfd, events, epoll_max_events, -1);
        errno_assert (rc != -1);

        // Process commands from other threads first.
        for (struct epoll_event *ep = events; ep < &events [rc]; ep++)

            //  Check for commands from other threads.
            if (ep->data.ptr == NULL && ep->events & EPOLLIN) {
                uint32_t signals = signaler.check ();
                assert (signals);
                if (!process_commands (signals))
                    return;
            }
            else {
                i_pollable *engine;
                engine = ((struct poll_entry *) ep->data.ptr)->engine;

                //  Process out events from the engine.
                if (ep->events & (EPOLLERR | EPOLLHUP))
                    engine->error_event ();

                //  Process out events from the engine.
                if (ep->events & EPOLLOUT)
                    engine->out_event();

                //  Process in events from the engine.
                if (ep->events & EPOLLIN)
                    engine->in_event();
            }
    }
}

bool zmq::epoll_thread_t::process_command (const command_t &command_)
{
    i_engine *engine;

    switch (command_.type) {

    //  Exit the working thread.
    case command_t::stop:
        return false;

    //  Register the engine supplied with the poll thread.
    case command_t::register_engine:
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
        engine->process_command (command_.args.engine_command.command);
        return true;

    //  Unknown command.
    default:
        assert (false);
        return false;
    }
}

bool epoll_thread_t::process_commands (uint32_t signals_)
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
