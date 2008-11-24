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

#ifndef __ZMQ_I_EVENT_MONITOR_HPP_INCLUDED__
#define __ZMQ_I_EVENT_MONITOR_HPP_INCLUDED__

#include <vector>

namespace zmq
{
    union cookie_t {
        int fd;
        void *ptr;
    };

    enum events {
        //  The file contains some data.
        ZMQ_EVENT_IN,
        //  The file can accept some more data.
        ZMQ_EVENT_OUT,
        //  There was an error on file descriptor.
        ZMQ_EVENT_ERR
    };

    //  The wait() method uses this structure to pass an event to its caller.
    struct event_t {
        int fd;
        enum events name;
        void *udata;
    };

    typedef std::vector <event_t> event_list_t;

    struct i_event_monitor
    {
        //  Add file descriptor to the polling set. Return handle
        //  representing the descriptor.
        virtual cookie_t add_fd (int fd_, void *udata_) = 0;

        //  Remove file descriptor identified by handle from the polling set.
        virtual void rm_fd (cookie_t cookie_) = 0;

        //  Start polling for input from socket.
        virtual void set_pollin (cookie_t cookie_) = 0;

        //  Stop polling for input from socket.
        virtual void reset_pollin (cookie_t cookie_) = 0;

        //  Start polling for availability of the socket for writing.
        virtual void set_pollout (cookie_t cookie_) = 0;

        //  Stop polling for availability of the socket for writing.
        virtual void reset_pollout (cookie_t cookie_) = 0;

        //  Wait until one or more descriptors become ready.
        virtual void wait (event_list_t &event_list_) = 0;
    };

}

#endif
