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

#ifndef __ZMQ_TCP_LISTENER_HPP_INCLUDED__
#define __ZMQ_TCP_LISTENER_HPP_INCLUDED__

#include <zmq/export.hpp>
#include <zmq/stdint.hpp>
#include <zmq/fd.hpp>

namespace zmq
{
    //  The class encapsulating simple TCP listening socket.

    class tcp_listener_t
    {
    public:

        //  Create TCP listining socket. Interface is either interface name,
        //  in that case port number is chosen by OS and can be retrieved
        //  by get_port method, or <interface-name>:<port-number>.
        ZMQ_EXPORT tcp_listener_t (const char *interface_);

        //  Closes the socket.
        ZMQ_EXPORT ~tcp_listener_t ();

        //  Get the file descriptor to poll on to get notified about
        //  newly created connections.
        inline fd_t get_fd ()
        {
            return s;
        }

        //  Returns port listener is listening on. The pointer returned is
        //  valid only while the listener object exists.
        inline const char *get_interface ()
        {
            return iface; 
        }

        //  Accept the new connection.
        fd_t accept ();

    private:

        //  Name of the interface listenet is listening on.
        char iface [256];

        //  Underlying socket.
        fd_t s;

        tcp_listener_t (const tcp_listener_t&);
        void operator = (const tcp_listener_t&);
    };

}

#endif
