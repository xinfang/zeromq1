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
#include <string.h>
#include <string>

#include "config.h"
#ifdef ZMQ_HAVE_WINXP
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include "err.hpp"
#include "ip.hpp"

#ifdef ZMQ_HAVE_WINDOWS

zmq::tcp_listener_t::tcp_listener_t (const char *interface_)
{
    //  Convert the hostname into sockaddr_in structure.
    sockaddr_in ip_address;
    resolve_ip_interface (&ip_address, interface_);
    
    //  Create a listening socket.
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //  TODO: check error

    //  Allow reusing of the address.
    int flag = 1;
    int rc = setsockopt (s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof (int));
    //  TODO: check error

    //  Bind the socket to the network interface and port.
    rc = bind (s, (struct sockaddr*) &ip_address, sizeof (ip_address));
    //  TODO: check error

    //  If port number was not specified, retrieve the one assigned
    //  to the socket by the operating system.
    if (ntohs (ip_address.sin_port) == 0) {

        sockaddr_in addr;
        memset (&addr, 0, sizeof (sockaddr_in));
        socklen_t sz = sizeof (sockaddr_in);
        int rc = getsockname (s, (sockaddr*) &addr, &sz);
        assert (rc == 0);
        ip_address.sin_port = addr.sin_port;
    }

    //  Fill in the interface name.
    const char *rcp = inet_ntop (AF_INET, &ip_address.sin_addr, interface,
        sizeof (interface));
    assert (rcp);
    size_t isz = strlen (interface);
    _snprintf_s (iface + isz, sizeof (iface) - isz, _TRUNCATE, ":%d",
        (int) ntohs (ip_address.sin_port));   
              
    //  Listen for incomming connections.
    rc = listen (s, 1);
    //  TODO: check error
}

int zmq::tcp_listener_t::accept ()
{
    //  Accept one incoming connection.
    int res = ::accept (s, NULL, NULL);
    //  TODO: check error
    return res;
}

#else

zmq::tcp_listener_t::tcp_listener_t (const char *interface_)
{
    //  Convert the hostname into sockaddr_in structure.
    sockaddr_in ip_address;
    resolve_ip_interface (&ip_address, interface_);
    
    //  Create a listening socket.
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    errno_assert (s != -1);

    //  Allow reusing of the address.
    int flag = 1;
    int rc = setsockopt (s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Bind the socket to the network interface and port.
    rc = bind (s, (struct sockaddr*) &ip_address, sizeof (ip_address));
    errno_assert (rc == 0);

    //  If port number was not specified, retrieve the one assigned
    //  to the socket by the operating system.
    if (ntohs (ip_address.sin_port) == 0) {

        sockaddr_in addr;
        memset (&addr, 0, sizeof (sockaddr_in));
        socklen_t sz = sizeof (sockaddr_in);
        int rc = getsockname (s, (sockaddr*) &addr, &sz);
        assert (rc == 0);
        ip_address.sin_port = addr.sin_port;
    }

    //  Fill in the interface name.
    const char *rcp = inet_ntop (AF_INET, &ip_address.sin_addr, interface,
        sizeof (interface));
    assert (rcp);
    size_t isz = strlen (interface);
    snprintf (interface + isz, sizeof (interface) - isz, ":%d",
        (int) ntohs (ip_address.sin_port));
              
    //  Listen for incomming connections.
    rc = listen (s, 1);
    errno_assert (rc == 0);
}

int zmq::tcp_listener_t::accept ()
{
    //  Accept one incoming connection.
    int res = ::accept (s, NULL, NULL);
    errno_assert (res != -1);
    return res;
}

#endif
