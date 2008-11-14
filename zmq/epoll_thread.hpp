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

#ifndef __ZMQ_EPOLL_THREAD_HPP_INCLUDED__
#define __ZMQ_EPOLL_THREAD_HPP_INCLUDED__

#include "platform.hpp"
#ifdef ZMQ_HAVE_LINUX

#include <map>
#include <sys/epoll.h>

#include "i_pollable.hpp"
#include "i_poller.hpp"
#include "dispatcher.hpp"
#include "ysocketpair.hpp"
#include "thread.hpp"

namespace zmq
{

    //  Epoll thread is an I/O thread that waits for events from engines
    //  using Linux-specific epoll function and schedules handling of the
    //  signals by individual engines. An engine compatible with epoll
    //  thread should implement i_pollable interface.

    class epoll_thread_t : public i_poller
    {
    public:

        //  Create epoll thread.
        static i_thread *create (dispatcher_t *dispatcher_);

        //  Destroy the poll thread.
        ~epoll_thread_t ();

        //  i_poller implementation.
        int get_thread_id ();
        void send_command (i_thread *destination_, const command_t &command_);
        handle_t add_fd (int fd_, i_pollable *engine_);
        void rm_fd (handle_t handle_);
        void set_pollin (handle_t handle_);
        void reset_pollin (handle_t handle_);
        void set_pollout (handle_t handle_);
        void reset_pollout (handle_t handle_);

    private:

        epoll_thread_t (dispatcher_t *dispatcher_);

        //  Main worker thread routine.
        static void worker_routine (void *arg_);

        //  Main routine (non-static) - called from worker_routine.
        void loop ();

        //  Processes individual command. Returns false if the thread should
        //  terminate.
        bool process_command (const command_t &command_);

        //  Processes commands from other threads. Returns false if the thread
        //  should terminate.
        bool process_commands (uint32_t signals_);

        // Epoll file descriptor
        int epfd;

        //  Pointer to dispatcher.
        dispatcher_t *dispatcher;

        //  Thread ID allocated for the poll thread by dispatcher.
        int thread_id;

        //  We get notifications about incoming commands using
        //  this socketpair.
        ysocketpair_t signaler;

        //  Handle of the physical thread doing the I/O work.
        thread_t *worker;

        // poll_entry
        struct poll_entry {
            int fd;
            struct epoll_event ev;
            i_pollable *engine;
        };

        //  All registered engines are stored in a common table.
        typedef std::map <void*, int> object_table_t;
        object_table_t engines;

        epoll_thread_t (const epoll_thread_t&);
        void operator = (const epoll_thread_t&);
    };

}

#endif

#endif
