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

#ifndef __ZMQ_SELECT_THREAD_HPP_INCLUDED__
#define __ZMQ_SELECT_THREAD_HPP_INCLUDED__

#include <zmq/platform.hpp>

#include <stddef.h>
#include <vector>

#ifdef ZMQ_HAVE_WINDOWS
#include "winsock2.h"
#else
#include <sys/select.h>
#endif

#include <zmq/poller.hpp>

namespace zmq
{

    //  Implements socket polling mechanism using POSIX.1-2001 select()
    //  function. The class is used to instatntiate the poller template
    //  to generate the select_thread_t class.

    class select_t
    {
    public:

        ZMQ_EXPORT select_t ();

        ZMQ_EXPORT cookie_t add_fd (int fd_, event_source_t *ev_source_);
        ZMQ_EXPORT void rm_fd (cookie_t cookie_);
        ZMQ_EXPORT void set_pollin (cookie_t cookie_);
        ZMQ_EXPORT void reset_pollin (cookie_t cookie_);
        ZMQ_EXPORT void set_pollout (cookie_t cookie_);
        ZMQ_EXPORT void reset_pollout (cookie_t cookie_);
        ZMQ_EXPORT bool process_events (poller_t <select_t> *poller_);

    private:

        struct fd_entry_t {
            int fd;
            event_source_t *ev_source;
        };

        //  Set of file descriptors that are used to retreive
        //  information for fd_set.
        typedef std::vector <fd_entry_t> fd_set_t;
        fd_set_t fds;

        fd_set source_set_in;
        fd_set source_set_out;
        fd_set source_set_err;

        fd_set readfds;
        fd_set writefds;
        fd_set exceptfds;

        //  Maximum file descriptor.
        int maxfd;

        //  If true, at least one file descriptor is retired.
        bool retired;

        select_t (const select_t&);
        void operator = (const select_t&);
    };

    typedef poller_t <select_t> select_thread_t;

}

#endif

