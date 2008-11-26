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

#include "platform.hpp"

#include <algorithm>
#include <string.h>

#ifdef ZMQ_HAVE_WINDOWS
#include "winsock2.h"
#else
#include <sys/select.h>
#endif

#include <zmq/err.hpp>
#include <zmq/select_thread.hpp>

zmq::select_t::select_t () :
    fd_table (FD_SETSIZE), maxfd (-1)
{
    //  Clear file descriptor sets.
    FD_ZERO (&source_set_in);
    FD_ZERO (&source_set_out);
    FD_ZERO (&source_set_err);

    for (int i = 0; i < FD_SETSIZE; i ++)
        fd_table [i].in_use = false;
}

zmq::cookie_t zmq::select_t::add_fd (int fd_, event_source_t *ev_source_)
{
    assert (!fd_table [fd_].in_use);
    fd_table [fd_].ev_source = ev_source_;
    fd_table [fd_].in_use = true;

    fds.push_back (fd_);
    FD_SET (fd_, &source_set_err);

    //  Adjust maxfd if necessary.
    if (fd_ > maxfd)
        maxfd = fd_;

    cookie_t cookie;
    cookie.fd = fd_;
    return cookie;
}

void zmq::select_t::rm_fd (cookie_t cookie_)
{
    //  Mark descriptor as unused.
    int fd = cookie_.fd;
    fd_table [fd].in_use = false;

    //  Update descriptor sets.
    FD_CLR (fd, &source_set_in);
    FD_CLR (fd, &source_set_out);
    FD_CLR (fd, &source_set_err);

    //  Adjust the maxfd attribute if we have removed the
    //  highest-numbered file descriptor.
    while (maxfd >= 0 && !fd_table [maxfd].in_use)
        maxfd --;

    fd_set_t::iterator it = std::find (fds.begin (), fds.end (), fd);
    assert (it != fds.end ());
    fds.erase (it);
}

void zmq::select_t::set_pollin (cookie_t cookie_)
{
    FD_SET (cookie_.fd, &source_set_in);
}

void zmq::select_t::reset_pollin (cookie_t cookie_)
{
    FD_CLR (cookie_.fd, &source_set_in);
}

void zmq::select_t::set_pollout (cookie_t cookie_)
{
    FD_SET (cookie_.fd, &source_set_out);
}

void zmq::select_t::reset_pollout (cookie_t cookie_)
{
    FD_CLR (cookie_.fd, &source_set_out);
}

void zmq::select_t::wait (event_list_t &event_list)
{
    memcpy (&readfds, &source_set_in, sizeof source_set_in);
    memcpy (&writefds, &source_set_out, sizeof source_set_out);
    memcpy (&exceptfds, &source_set_err, sizeof source_set_err);

    while (true) {
        int rc = select (maxfd + 1, &readfds, &writefds, &exceptfds, NULL);
#ifdef ZMQ_HAVE_WINDOWS
        wsa_assert (rc != SOCKET_ERROR);
#else
        errno_assert (rc != -1);
#endif
        if (rc > 0)
            break;
    }

    for (fd_set_t::size_type i = 0; i < fds.size (); i ++) {
        if (FD_ISSET (fds [i], &writefds)) {
            event_t ev = {fds [i], ZMQ_EVENT_OUT, fd_table [fds [i]].ev_source};
            event_list.push_back (ev);
        }
        if (FD_ISSET (fds [i], &readfds)) {
            event_t ev = {fds [i], ZMQ_EVENT_IN, fd_table [fds [i]].ev_source};
            event_list.push_back (ev);
        }
        if (FD_ISSET (fds [i], &exceptfds)) {
            event_t ev = {fds [i], ZMQ_EVENT_ERR, fd_table [fds [i]].ev_source};
            event_list.push_back (ev);
        }
    }
}
