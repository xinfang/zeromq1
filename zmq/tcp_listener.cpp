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

#include "tcp_listener.hpp"

#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "err.hpp"

zmq::tcp_listener_t::tcp_listener_t (const char *interface_, uint16_t port_)
{
    //  Create IP address of the network interface
    struct addrinfo req;
    memset (&req, 0, sizeof req);
    struct addrinfo *res;
    req.ai_family = AF_INET;
    int rc = getaddrinfo (interface_, NULL, &req, &res);
    gai_assert (rc);
    sockaddr_in ip_address = *((sockaddr_in *) res->ai_addr);
    freeaddrinfo (res);
    ip_address.sin_port = htons (port_);

    //  Create a listening socket
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    errno_assert (s != -1);

    //  Allow reusing of the address
    int flag = 1;
    rc = setsockopt (s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Bind the socket to the network interface and port
    rc = bind (s, (struct sockaddr*) &ip_address, 
        sizeof (ip_address));
    errno_assert (rc == 0);
              
    //  Listen for incomming connections
    rc = listen (s, 1);
    errno_assert (rc == 0);
}

int zmq::tcp_listener_t::accept ()
{
    //  Accept one incoming connection
    int res = ::accept (s, NULL, NULL);
    errno_assert (res != -1);
    return res;
}
