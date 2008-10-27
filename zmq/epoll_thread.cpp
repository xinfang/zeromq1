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

#include "epoll_thread.hpp"

#ifdef ZMQ_HAVE_LINUX

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
    dispatcher->write (thread_id, destination_->get_thread_id (), command_);
}

epoll_thread_t::poll_entry *
epoll_thread_t::new_poll_entry (int fd_, i_pollable *engine_)
{
    struct poll_entry *poll_entry;

    poll_entry = (struct poll_entry*) malloc (sizeof (struct poll_entry));
    if (poll_entry) {
        poll_entry->ev.events = 0;
        poll_entry->ev.data.ptr = poll_entry;
        poll_entry->fd = fd_;
        poll_entry->engine = engine_;
    }

    return poll_entry;
}

handle_t epoll_thread_t::add_fd (int fd_, i_pollable *engine_)
{
    handle_t handle;

    struct poll_entry *poll_entry = new_poll_entry (fd_, engine_);
    assert (poll_entry != NULL);

    int rc = epoll_ctl (epfd, EPOLL_CTL_ADD, fd_, &poll_entry->ev);
    errno_assert (rc != -1);

    object_table_t::iterator it = engines.find (engine_);
    if (it == engines.end ())
        engines.insert (object_table_t::value_type (engine_, 1));
    else
        it->second++;

    handle.ptr = poll_entry;
    return handle;
}

void epoll_thread_t::rm_fd (handle_t handle_)
{
    struct poll_entry *poll_entry = (struct poll_entry *) handle_.ptr;

    object_table_t::iterator it = engines.find (poll_entry->engine);
    assert (it != engines.end ());

    if (--it->second == 0)
        engines.erase (it);

    int rc = epoll_ctl (epfd, EPOLL_CTL_DEL, poll_entry->fd, &poll_entry->ev);
    errno_assert (rc != -1);

    free (poll_entry);
}

void epoll_thread_t::set_pollin (handle_t handle_)
{
    struct poll_entry *poll_entry = (struct poll_entry*) handle_.ptr;
    poll_entry->ev.events |= EPOLLIN;
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, poll_entry->fd, &poll_entry->ev);
    errno_assert (rc != -1);
}

void epoll_thread_t::reset_pollin (handle_t handle_)
{
    struct poll_entry *poll_entry = (struct poll_entry*) handle_.ptr;
    poll_entry->ev.events &= ~((short) EPOLLIN);
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, poll_entry->fd, &poll_entry->ev);
    errno_assert (rc != -1);
}

void epoll_thread_t::speculative_read (handle_t handle_)
{
    struct poll_entry *poll_entry = (struct poll_entry*) handle_.ptr;
    poll_entry->ev.events |= EPOLLIN;
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, poll_entry->fd, &poll_entry->ev);
    errno_assert (rc != -1);
    poll_entry->engine->in_event ();
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
    struct poll_entry *poll_entry = (struct poll_entry*) handle_.ptr;
    poll_entry->ev.events &= ~((short) EPOLLOUT);
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, poll_entry->fd, &poll_entry->ev);
    errno_assert (rc != -1);
}

void epoll_thread_t::speculative_write (handle_t handle_)
{
    struct poll_entry *poll_entry = (struct poll_entry*) handle_.ptr;
    poll_entry->ev.events |= EPOLLOUT;
    int rc = epoll_ctl (epfd, EPOLL_CTL_MOD, poll_entry->fd, &poll_entry->ev);
    errno_assert (rc != -1);
    poll_entry->engine->out_event ();
}

void epoll_thread_t::worker_routine (void *arg_)
{
    epoll_thread_t *self = (epoll_thread_t*) arg_;
    self->loop ();
}

void epoll_thread_t::loop ()
{
    struct epoll_event events[epoll_max_events];

    while (true) {

        //  Wait for events.
        int rc = epoll_wait (epfd, events, epoll_max_events, -1);
        errno_assert (rc != -1);

        // Process commands from other threads first.
        for (struct epoll_event *ep = events; ep < &events[rc]; ep++)

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

bool epoll_thread_t::process_commands (uint32_t signals_)
{
    i_engine *engine;

    //  Iterate through all the threads in the process and find out which
    //  of them sent us commands.
    for (int i = 0; i != dispatcher->get_thread_count (); i++)
        if (signals_ & 1 << i) {

            //  Read all the commands from particular thread.
            command_t command;
            while (dispatcher->read (i, thread_id, &command))

                switch (command.type) {

                //  Exit the working thread.
                case command_t::stop:
                    return false;

                //  Register the engine supplied with the poll thread.
                case command_t::register_engine:
                    engine = command.args.register_engine.engine;
                    assert (engine->type () == engine_type_fd);
                    ((i_pollable*) engine)->register_event (this);
                    break;

                //  Unregister the engine.
                case command_t::unregister_engine:

                    //  Assert that engine still exists.
                    //  TODO: We should somehow make sure this won't happen.
                    engine = command.args.unregister_engine.engine;
                    assert (engines.find (engine) != engines.end ());

                    //  Ask engine to unregister itself.
                    assert (engine->type () == engine_type_fd);
                    ((i_pollable*) engine)->unregister_event ();
                    break;

                //  Forward the command to the specified engine.
                case command_t::engine_command:

                    //  Check whether engine still exists.
                    //  TODO: We should somehow make sure this won't happen.

                    //  Forward the command to the engine.
                    //  TODO: If the engine doesn't exist drop the command.
                    //        However, imagine there's another engine
                    //        incidentally allocated on the same address.

                    engine = command.args.register_engine.engine;
                    if (engines.find (engine) != engines.end ())
                        engine->process_command (
                            command.args.engine_command.command);
                    break;

                //  Unknown command.
                default:
                    assert (false);
                }
        }
    return true;
}

#endif
