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
#include "tcp_listener.hpp"

namespace zmq
{

    //  The class encapsulating simple TCP read/write socket.

    class tcp_socket_t
    {
    public:

        //  Opens TCP socket. Hostname should be in form of <host>:<port>.
        //  By default it opens the socket in non-blocking mode. If block_ is
        //  set to true, the socket will be opened in blocking mode.
        tcp_socket_t (const char *hostname_, bool block_ = false);

        //  Opens a socket by accepting a connection from TCP listener object
        tcp_socket_t (tcp_listener_t &listener);
         
        //  Closes the socket.
        ~tcp_socket_t ();

        //  Returns the underlying socket.
        inline int get_fd ()
        {
            return s;
        }

        //  Writes data to the socket. Returns the number
        //  of bytes actually written.
        size_t write (const void *data, size_t size);

        //  Reads data from the socket (up to 'size' bytes). Returns the number
        //  of bytes actually read.
        size_t read (void *data, size_t size);

    private:

        //  Underlying socket
        int s;
        bool block;

        //  Disable copy construction of tcp_socket.
        tcp_socket_t (const tcp_socket_t&);
        void operator = (const tcp_socket_t&);
    };

}

#endif
