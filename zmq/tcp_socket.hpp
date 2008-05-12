/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_TCP_SOCKET_HPP_INCLUDED__
#define __ZMQ_TCP_SOCKET_HPP_INCLUDED__

#include <stddef.h>

#include "stdint.hpp"

namespace zmq
{

    //  Encapsulates simple TCP socket
    class tcp_socket_t
    {
    public:

        //  Attaches object to the existing socket
        inline tcp_socket_t (int sock_) :
            listening_socket (-1),
            s (sock_)
        {
        }

        // Opens a TCP socket. Either connectes to the host specified by
        // 'address' argument (when listen = false), or listens to the incoming
        // connections on the network interface specified by 'address'
        // (when listen = true).
        tcp_socket_t (bool listen_, const char *address_, uint16_t port_);

        //  Closes the socket
        ~tcp_socket_t ();

        //  Returns the underlying raw socket
        int get_fd ();

        //  Writes as much data as possible to the socket. Returns the number
        //  of bytes actually written.
        size_t write (const void *data, size_t size);

        //  Reads data from the socket (up to 'size' bytes). Returns the number
        //  of bytes actually read.
        size_t read (void *data, size_t size);

        //  Writes all the data to the socket
        void blocking_write (const void *data, size_t size);

        //  Reads 'size' bytes from the socket
        void blocking_read (void *data, size_t size);

    private:
        int listening_socket;
        int s;
    };

}

#endif
