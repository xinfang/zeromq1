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
#include <string>

#include "stdint.hpp"
#include "tcp_listener.hpp"

namespace zmq
{

    //  The class encapsulating simple TCP read/write socket.

    class tcp_socket_t
    {
    public:

        //  Opens TCP socket. Hostname should be in form of <host>:<port>.
        tcp_socket_t (const char *hostname_);

        //  Opens a socket by accepting a connection from TCP listener object
        tcp_socket_t (tcp_listener_t &listener);
         
        //  Closes the socket.
        ~tcp_socket_t ();

        //  Returns true if the connection was established using
        //  the connect() call and hence can be re-established on error.
        bool is_reconnectable ();

        //  Closes the connection to the host and reconnects anew. If hostname
        //  in NULL, socket reconnects to the same host. Othewise it reconnects
        //  to the host supplied.
        void reconnect (const char *hostname_ = NULL);

        //  Returns the underlying socket.
        inline int get_fd ()
        {
            return s;
        }

        // Writes data to the socket. Returns the number of bytes actually
        // written (even zero is to be considered to be a success). In case
        // of orderly shutdown by the other peer -1 is returned.
        int write (const void *data, int size);

        // of bytes actually read (even zero is to be considered to be
        // a success). In case of orderly shutdown by the other peer -1 is
        // returned.
        int read (void *data, int size);

        //  Writes all the data to the socket.
        void blocking_write (const void *data, size_t size);

        //  Reads 'size' bytes from the socket.
        void blocking_read (void *data, size_t size);

        void send_string (const std::string &s);

        std::string recv_string (size_t maxlen);

    private:

         void connect ();

        //  Underlying socket
        int s;

        //  Stores the name of the host to connect to in case of reconnection.
        std::string hostname;

        //  Disable copy construction of tcp_socket.
        tcp_socket_t (const tcp_socket_t&);
        void operator = (const tcp_socket_t&);
    };

}

#endif
