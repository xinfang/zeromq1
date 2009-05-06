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

#ifndef __ZMQ_DEVPOLL_THREAD_HPP_INCLUDED__
#define __ZMQ_DEVPOLL_THREAD_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_SOLARIS || ZMQ_HAVE_HPUX

#include <vector>

#include <zmq/i_poller.hpp>
#include <zmq/poller.hpp>
#include <zmq/fd.hpp>

namespace zmq
{

    //  Implements socket polling mechanism using the Solaris-specific
    //  "/dev/poll" interface.

    class devpoll_t : public i_poller
    {
    public:

        devpoll_t ();
        ~devpoll_t ();

        //  i_poller implementation.
        handle_t add_fd (fd_t fd_, i_pollable *engine_);
        void rm_fd (handle_t handle_);
        void set_pollin (handle_t handle_);
        void reset_pollin (handle_t handle_);
        void set_pollout (handle_t handle_);
        void reset_pollout (handle_t handle_);
        void add_timer (i_pollable *engine_);
        void cancel_timer (i_pollable *engine_);

        void process_events ();

    private:

        //  File descriptor referring to "/dev/poll" pseudo-device.
        fd_t devpoll_fd;

        struct fd_entry_t
        {
            short events;
            i_pollable *engine;
            bool in_use;
            bool adopted;
        };

        std::vector <fd_entry_t> fd_table;

        typedef std::vector <fd_t> pending_list_t;
        pending_list_t pending_list;

        //  List of all the engines waiting for the timer event.
        typedef std::vector <i_pollable*> timers_t;
        timers_t timers;

        //  Pollset manipulation function.
        void devpoll_ctl (fd_t fd_, short events_);

        devpoll_t (const devpoll_t&);
        void operator = (const devpoll_t&);
    };

    typedef poller_t <devpoll_t> devpoll_thread_t;

}

#endif

#endif
