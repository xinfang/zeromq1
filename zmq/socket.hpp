/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __ZMQ_SOCKET_INCLUDED__
#define __ZMQ_SOCKET_INCLUDED__

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include "err.hpp"

namespace zmq
{

    //  Encapsulates simple TCP socket
    class socket_t
    {
    public:

        // Opens a TCP socket. Either connectes to the host specified by
        // 'address' argument (when listen = false), or listens to the incoming
        // connections on the network interface specified by 'address'
        // (when listen = true).
        inline socket_t (bool listen, const char *address, unsigned short port)
        {
            //  Create IP addess
            sockaddr_in ip_address;
            memset (&ip_address, 0, sizeof (ip_address));
            ip_address.sin_family = AF_INET;
            int rc = inet_pton (AF_INET, address, &ip_address.sin_addr);
            errno_assert (rc > 0);
            ip_address.sin_port = htons (port);

            if (listen) {

                //  Create a listening socket
                listening_socket = socket (AF_INET, SOCK_STREAM,
                    IPPROTO_TCP);
                errno_assert (listening_socket != -1);

                //  Allow socket reusing
                int flag = 1;
                rc = setsockopt (listening_socket, SOL_SOCKET, SO_REUSEADDR,
                    &flag, sizeof (int));
                errno_assert (rc == 0);

                //  Bind the socket to the network interface and port
                rc = bind (listening_socket, (struct sockaddr*) &ip_address,
                    sizeof (ip_address));
                errno_assert (rc == 0);
              
                //  Listen for incomming connections
                rc = ::listen (listening_socket, 1);
                errno_assert (rc == 0);

                //  Accept first incoming connection
                s = accept (listening_socket, NULL, NULL);
                errno_assert (s != -1);
            }
            else {

                //  Mark listening socket as unused
                listening_socket = -1;

                //  Create the socket
                s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
                errno_assert (s != -1);

                //  Connect to the remote peer
                rc = connect (s, (sockaddr*) &ip_address, sizeof (ip_address));
                errno_assert (rc != -1);
            }

            //  Disable Nagle's algorithm
            int flag = 1;
            rc = setsockopt (s, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof (int));
            errno_assert (rc == 0);
        }

        //  Closes the socket
        inline ~socket_t ()
        {
            int rc = close (s);
            errno_assert (rc == 0);

            if (listening_socket != -1) {
                rc = close (listening_socket);
                errno_assert (rc == 0);
            }
        }

        //  Returns the underlying raw socket
        inline operator int ()
        {
            return s;
        }
    protected:
        int listening_socket;
        int s;
    };

};

#endif
