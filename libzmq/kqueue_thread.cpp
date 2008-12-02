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

#if defined (ZMQ_HAVE_FREEBSD) || defined (ZMQ_HAVE_OPENBSD) ||\
    defined (ZMQ_HAVE_OSX)

#include <sys/time.h>
#include <sys/types.h>
#include <sys/event.h>
#include <stdlib.h>
#include <unistd.h>

#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/kqueue_thread.hpp>

zmq::kqueue_t::kqueue_t ()
{
    //  Create event queue
    kqueue_fd = kqueue ();
    errno_assert (kqueue_fd != -1);
}

zmq::kqueue_t::~kqueue_t ()
{
    close (kqueue_fd);
}

void zmq::kqueue_t::kevent_add (int fd_, short filter_, void *udata_)
{
    struct kevent ev;

    EV_SET (&ev, fd_, filter_, EV_ADD, 0, 0, udata_);
    int rc = kevent (kqueue_fd, &ev, 1, NULL, 0, NULL);
    errno_assert (rc != -1);
}

void zmq::kqueue_t::kevent_delete (int fd_, short filter_)
{
    struct kevent ev;

    EV_SET (&ev, fd_, filter_, EV_DELETE, 0, 0, NULL);
    int rc = kevent (kqueue_fd, &ev, 1, NULL, 0, NULL);
    errno_assert (rc != -1);
}

zmq::cookie_t zmq::kqueue_t::add_fd (int fd_, event_source_t *ev_source_)
{
    struct poll_entry *pe;
    pe = (struct poll_entry*) malloc (sizeof (struct poll_entry));
    assert (pe != NULL);
    pe->fd = fd_;
    pe->flag_pollin = 0;
    pe->flag_pollout = 0;
    pe->ev_source = ev_source_;

    cookie_t cookie;
    cookie.ptr = pe;
    return cookie;
}

void zmq::kqueue_t::rm_fd (cookie_t cookie_)
{
    struct poll_entry *pe = (struct poll_entry*) cookie_.ptr;
    if (pe->flag_pollin)
        kevent_delete (pe->fd, EVFILT_READ);
    if (pe->flag_pollout)
        kevent_delete (pe->fd, EVFILT_WRITE);
    pe->fd = -1;
    retired.push_back (pe);
}

void zmq::kqueue_t::set_pollin (cookie_t cookie_)
{
    struct poll_entry *pe = (struct poll_entry*) cookie_.ptr;
    pe->flag_pollin = true;
    kevent_add (pe->fd, EVFILT_READ, pe);
}

void zmq::kqueue_t::reset_pollin (cookie_t cookie_)
{
    struct poll_entry *pe = (struct poll_entry*) cookie_.ptr;
    pe->flag_pollin = false;
    kevent_delete (pe->fd, EVFILT_READ);
}

void zmq::kqueue_t::set_pollout (cookie_t cookie_)
{
    struct poll_entry *pe = (struct poll_entry*) cookie_.ptr;
    pe->flag_pollout = true;
    kevent_add (pe->fd, EVFILT_WRITE, pe);
}

void zmq::kqueue_t::reset_pollout (cookie_t cookie_)
{
    struct poll_entry *pe = (struct poll_entry*) cookie_.ptr;
    pe->flag_pollout = false;
    kevent_delete (pe->fd, EVFILT_WRITE);
}

bool zmq::kqueue_t::process_events (poller_t <kqueue_t> *poller_)
{
    struct kevent ev_buf [max_io_events];

    //  Wait for events.
    int n = kevent (kqueue_fd, NULL, 0,
        &ev_buf [0], max_io_events, NULL);
    errno_assert (n != -1);

    for (int i = 0; i < n; i ++) {
        struct poll_entry *pe = (struct poll_entry*) ev_buf [i].udata;
        event_source_t *ev_source = pe->ev_source;

        if (pe->fd == -1)
            continue;
        if (ev_buf [i].flags & EV_EOF)
            if (poller_->process_event (ev_source, ZMQ_EVENT_ERR))
                return true;
        if (pe->fd == -1)
            continue;
        if (ev_buf [i].filter == EVFILT_WRITE)
            if (poller_->process_event (ev_source, ZMQ_EVENT_OUT))
                return true;
        if (pe->fd == -1)
            continue;
        if (ev_buf [i].filter == EVFILT_READ)
            if (poller_->process_event (ev_source, ZMQ_EVENT_IN))
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
