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

#include "tcp_socket.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.hpp>

#include "err.hpp"

zmq::tcp_socket_t::tcp_socket_t (bool listen_, const char *address_,
    uint16_t port_)
{
    //  Create IP addess
    sockaddr_in ip_address;
    memset (&ip_address, 0, sizeof (ip_address));
    ip_address.sin_family = AF_INET;
    int rc = inet_pton (AF_INET, address_, &ip_address.sin_addr);
    errno_assert (rc > 0);
    ip_address.sin_port = htons (port_);

    if (listen_) {

        //  Create a listening socket
        listening_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

zmq::tcp_socket_t::~tcp_socket_t ()
{
    int rc = close (s);
    errno_assert (rc == 0);

    if (listening_socket != -1) {
        rc = close (listening_socket);
        errno_assert (rc == 0);
    }
}

int zmq::tcp_socket_t::get_fd ()
{
    return s;
}

size_t zmq::tcp_socket_t::write (unsigned char *data, size_t size)
{
    ssize_t nbytes = send (s, data, size, MSG_DONTWAIT);
    errno_assert (nbytes != -1);
    return (size_t) nbytes;
}

size_t zmq::tcp_socket_t::read (unsigned char *data, size_t size)
{
    ssize_t nbytes = recv (s, data, size, MSG_DONTWAIT);
    errno_assert (nbytes != -1);
    return (size_t) nbytes;
}

void zmq::tcp_socket_t::blocking_write (unsigned char *data, size_t size)
{
    ssize_t nbytes = send (s, data, size, 0);
    errno_assert (nbytes == size);
}

void zmq::tcp_socket_t::blocking_read (unsigned char *data, size_t size)
{
    ssize_t nbytes = recv (s, data, size, MSG_WAITALL);
    errno_assert (nbytes == size);
}

