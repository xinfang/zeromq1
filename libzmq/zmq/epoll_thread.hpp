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

#include <zmq/platform.hpp>

#ifdef ZMQ_HAVE_LINUX

#include <sys/epoll.h>
#include <vector>

#include <zmq/poller.hpp>

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

        cookie_t add_fd (int fd_, event_source_t *ev_source_);
        void rm_fd (cookie_t cookie_);
        void set_pollin (cookie_t cookie_);
        void reset_pollin (cookie_t cookie_);
        void set_pollout (cookie_t cookie_);
        void reset_pollout (cookie_t cookie_);
        bool process_events (poller_t <epoll_t> *poller_);

    private:

        // Epoll file descriptor
        int epoll_fd;

        // poll_entry
        struct poll_entry {
            int fd;
            struct epoll_event ev;
            event_source_t *ev_source;
        };

        epoll_t (const epoll_t&);
        void operator = (const epoll_t&);
    };

    typedef poller_t <epoll_t> epoll_thread_t;

}

#endif

#endif
