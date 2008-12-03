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

#include <zmq/platform.hpp>

#ifdef ZMQ_HAVE_LINUX

#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/epoll_thread.hpp>

zmq::epoll_t::epoll_t ()
{
    epoll_fd = epoll_create (1);
    errno_assert (epoll_fd != -1);
}

zmq::epoll_t::~epoll_t ()
{
    close (epoll_fd);
}

zmq::cookie_t zmq::epoll_t::add_fd (int fd_, event_source_t *ev_source_)
{
    poll_entry *pe = (poll_entry*) malloc (sizeof (poll_entry));
    assert (pe != NULL);
    pe->fd = fd_;
    pe->ev.events = 0;
    pe->ev.data.ptr = pe;
    pe->ev_source = ev_source_;

    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_ADD, fd_, &pe->ev);
    errno_assert (rc != -1);

    cookie_t cookie;
    cookie.ptr = pe;
    return cookie;
}

void zmq::epoll_t::rm_fd (cookie_t cookie_)
{
    poll_entry *pe = (poll_entry*) cookie_.ptr;
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_DEL, pe->fd, &pe->ev);
    errno_assert (rc != -1);
    pe->fd = -1;
    retired.push_back (pe);
}

void zmq::epoll_t::set_pollin (cookie_t cookie_)
{
    poll_entry *pe = (poll_entry*) cookie_.ptr;
    pe->ev.events |= EPOLLIN;
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void zmq::epoll_t::reset_pollin (cookie_t cookie_)
{
    poll_entry *pe = (poll_entry*) cookie_.ptr;
    pe->ev.events &= ~((short) EPOLLIN);
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void zmq::epoll_t::set_pollout (cookie_t cookie_)
{
    poll_entry *pe = (poll_entry*) cookie_.ptr;
    pe->ev.events |= EPOLLOUT;
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

void zmq::epoll_t::reset_pollout (cookie_t cookie_)
{
    poll_entry *pe = (poll_entry*) cookie_.ptr;
    pe->ev.events &= ~((short) EPOLLOUT);
    int rc = epoll_ctl (epoll_fd, EPOLL_CTL_MOD, pe->fd, &pe->ev);
    errno_assert (rc != -1);
}

bool zmq::epoll_t::process_events (poller_t <epoll_t> *poller_)
{
    struct epoll_event ev_buf [max_io_events];

    //  Wait for events.
    int n = epoll_wait (epoll_fd, &ev_buf [0], max_io_events, -1);
    errno_assert (n != -1);

    for (int i = 0; i < n; i ++) {
        poll_entry *pe = ((poll_entry*) ev_buf [i].data.ptr);

        if (pe->fd == -1)
            continue;
        if (ev_buf [i].events & (EPOLLERR | EPOLLHUP))
            if (poller_->process_event (pe->ev_source, event_err))
                return true;
        if (pe->fd == -1)
            continue;
        if (ev_buf [i].events & EPOLLOUT)
            if (poller_->process_event (pe->ev_source, event_out))
                return true;
        if (pe->fd == -1)
            continue;
        if (ev_buf [i].events & EPOLLIN)
            if (poller_->process_event (pe->ev_source, event_in))
                return true;
    }

    //  Destroy retired event sources.
    for (retired_t::iterator it = retired.begin (); it != retired.end ();
          it ++) {
        delete (*it)->ev_source;
        delete *it;
    }

    return false;
}

#endif
