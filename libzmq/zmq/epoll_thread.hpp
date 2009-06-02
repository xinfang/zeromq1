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

#ifndef __ZMQ_EPOLL_THREAD_HPP_INCLUDED__
#define __ZMQ_EPOLL_THREAD_HPP_INCLUDED__

#include <zmq/platform.hpp>

#ifdef ZMQ_HAVE_LINUX

#include <sys/epoll.h>
#include <vector>

#include <zmq/poller.hpp>
#include <zmq/fd.hpp>

namespace zmq
{

    //  Implements socket polling mechanism using the Linux-specific
    //  epoll mechanism. The class is used when  instantiating the poller
    //  template to generate the epoll_thread_t class.

    class epoll_t
    {
    public:

        epoll_t ();
        virtual ~epoll_t ();

        handle_t add_fd (fd_t fd_, i_pollable *engine_);
        void rm_fd (handle_t handle_);
        void set_pollin (handle_t handle_);
        void reset_pollin (handle_t handle_);
        void set_pollout (handle_t handle_);
        void reset_pollout (handle_t handle_);
        bool process_events (poller_t <epoll_t> *poller_, bool timers_);

    private:

        // Epoll file descriptor
        fd_t epoll_fd;

        struct poll_entry_t
        {
            fd_t fd;
            epoll_event ev;
            i_pollable *engine;
        };

        //  List of retired event sources.
        typedef std::vector <poll_entry_t*> retired_t;
        retired_t retired;

        epoll_t (const epoll_t&);
        void operator = (const epoll_t&);
    };

    typedef poller_t <epoll_t> epoll_thread_t;

}

#endif

#endif
