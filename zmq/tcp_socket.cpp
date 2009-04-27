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

#include "tcp_socket.hpp"
#include "wire.hpp"

#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "err.hpp"
#include "ip.hpp"

zmq::tcp_socket_t::tcp_socket_t (const char *hostname_) :
    hostname (hostname_)
{
    connect ();
}

zmq::tcp_socket_t::tcp_socket_t (tcp_listener_t &listener) :
    hostname ("")
{
    //  Accept the socket
    s = listener.accept ();
    errno_assert (s != -1);

    //  Disable Nagle's algorithm
    int flag = 1;
    int rc = setsockopt (s, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof (int));
    errno_assert (rc == 0);
}

zmq::tcp_socket_t::~tcp_socket_t ()
{
    int rc = close (s);
    errno_assert (rc == 0);
}

bool zmq::tcp_socket_t::is_reconnectable ()
{
    return !hostname.empty ();
}

void zmq::tcp_socket_t::reconnect (const char *hostname_)
{
    //  If new hostname is specified, store it.
    if (hostname_)
        hostname = hostname_;

    //  Connections created via listener cannot be reconnected.
    assert (hostname.size ());

    //  Close the old connection.
    int rc = close (s);
    errno_assert (rc == 0);

    //  Open the new one.
    connect ();
}

int zmq::tcp_socket_t::write (const void *data, int size)
{
    ssize_t nbytes = send (s, data, size, MSG_DONTWAIT);

    //  If not a single byte can be written to the socket in non-blocking mode
    //  we'll get an error (this may happen during the speculative write).
    if (nbytes == -1 && errno == EAGAIN)
        return 0;

    //  Signalise peer failure.
    if (nbytes == -1 && errno == ECONNRESET)
        return -1;

    errno_assert (nbytes != -1);
    return (size_t) nbytes;
}

int zmq::tcp_socket_t::read (void *data, int size)
{
    ssize_t nbytes = recv (s, data, size, MSG_DONTWAIT);

    //  If not a single byte can be read from the socket in non-blocking mode
    //  we'll get an error (this may happen during the speculative read).
    if (nbytes == -1 && errno == EAGAIN)
        return 0;

    //  Orderly shutdown by the other peer.
    if (nbytes == 0)
        return -1;

    errno_assert (nbytes != -1);
    return (size_t) nbytes;
}

void zmq::tcp_socket_t::blocking_write (const void *data, size_t size)
{
    size_t i = 0;

    while (i < size) {
        ssize_t nbytes = send (s, (char *) data + i, size - i, 0);
        errno_assert (nbytes > 0);
        i += nbytes;
    }
}

int zmq::tcp_socket_t::blocking_read (void *data, size_t size)
{
    size_t i = 0;

    while (i < size) {
        ssize_t nbytes = recv (s, (char *) data + i, size - i, MSG_WAITALL);
        if (nbytes == 0)
            return -1;
        errno_assert (nbytes > 0);
        i += nbytes;
    }

    return 0;
}

void zmq::tcp_socket_t::connect ()
{
    //  Convert the hostname into sockaddr_in structure.
    sockaddr_in ip_address;
    resolve_ip_hostname (&ip_address, hostname.c_str ());

    //  Connect to the remote peer. Do so in loop.
    while (true) {
        s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        errno_assert (s != -1);
        int rc = ::connect (s, (sockaddr *)&ip_address, sizeof (ip_address));

        //  Success.
        if (rc != -1)
            break;

        //  Temporary problem.
        if (rc == -1 && (errno == ECONNREFUSED || errno == EADDRNOTAVAIL ||
              errno == ENETUNREACH || errno == ETIMEDOUT)) {
            rc = close (s);
            errno_assert (rc == 0);

            //  Wait 10ms before trying to reconnect. This way we avoid
            //  exhausting CPU with attempts to reconnect when
            //  the peer is offline.
            usleep (10000);
            continue;
        }

        //  Permanent problem.
        assert (false);
    }

    //  Disable Nagle's algorithm.
    int flag = 1;
    int rc = setsockopt (s, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof (int));
    errno_assert (rc == 0);
}

void zmq::tcp_socket_t::send_string (const std::string &s)
{
    size_t slen = s.length ();

    if (slen < 255) {
        unsigned char tmpbuf [1];
        tmpbuf [0] = slen;
        blocking_write (tmpbuf, sizeof tmpbuf);
    }
    else {
        unsigned char tmpbuf [9];
        tmpbuf [0] = 0xff;
        put_uint64 (tmpbuf + 1, slen);
        blocking_write (tmpbuf, sizeof tmpbuf);
    }

    blocking_write (s.c_str (), slen);
}

int zmq::tcp_socket_t::recv_string (std::string &s, size_t maxlen)
{
    size_t slen;
    unsigned char tmpbuf [8];

    if (blocking_read (tmpbuf, 1) < 0)
        return -1;
    if (tmpbuf [0] != 0xff)
        slen = tmpbuf [0];
    else {
        if (blocking_read (tmpbuf, 8) < 0)
            return -1;
        slen = get_uint64 (tmpbuf);
    }

    if (slen > maxlen)
        return -1;

    char *buf = new char [slen];
    assert (buf);
    if (blocking_read (buf, slen) < 0) {
        delete [] buf;
        return -1;
    }
    s.assign (buf, slen);
    delete [] buf;

    return 0;
}
