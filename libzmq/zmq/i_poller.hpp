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

#ifndef __ZMQ_I_POLLER_HPP_INCLUDED__
#define __ZMQ_I_POLLER_HPP_INCLUDED__

#include <zmq/export.hpp>
#include <zmq/i_thread.hpp>

namespace zmq
{
    union handle_t
    {
        int fd;
        void *ptr;
    };

    //  Virtual interface to be exposed by file-descriptor-oriented engines
    //  for communication with I/O threads.

    struct i_poller : public i_thread
    {
        ZMQ_EXPORT virtual ~i_poller () {};

        //  Add file descriptor to the polling set. Return handle
        //  representing the descriptor.
        virtual handle_t add_fd (int fd_, struct i_pollable *engine_) = 0;

        //  Remove file descriptor identified by handle from the polling set.
        virtual void rm_fd (handle_t handle_) = 0;

        //  Start polling for input from socket.
        virtual void set_pollin (handle_t handle_) = 0;

        //  Stop polling for input from socket.
        virtual void reset_pollin (handle_t handle_) = 0;

        //  Start polling for availability of the socket for writing.
        virtual void set_pollout (handle_t handle_) = 0;

        //  Stop polling for availability of the socket for writing.
        virtual void reset_pollout (handle_t handle_) = 0;
    };

}

#endif
