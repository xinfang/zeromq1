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

#ifdef ZMQ_HAVE_SOLARIS

#include <sys/devpoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/devpoll_thread.hpp>

zmq::devpoll_t::devpoll_t ()
{
    struct rlimit rl;

    //  Get limit on open files
    int rc = getrlimit (RLIMIT_NOFILE, &rl);
    errno_assert (rc != -1);
    fd_table.resize (rl.rlim_cur);

    for (rlim_t i = 0; i < rl.rlim_cur; i ++)
        fd_table [i].in_use = false;

    devpoll_fd = open ("/dev/poll", O_RDWR);
    errno_assert (devpoll_fd != -1);
}

zmq::devpoll_t::~devpoll_t ()
{
    close (devpoll_fd);
}

void zmq::devpoll_t::devpoll_ctl (int fd_, short events_)
{
    struct pollfd pfd = {fd_, events_, 0};
    ssize_t rc = write (devpoll_fd, &pfd, sizeof pfd);
    errno_assert (rc == sizeof pfd);
}

zmq::cookie_t zmq::devpoll_t::add_fd (int fd_, event_source_t *ev_source_)
{
    assert (!fd_table [fd_].in_use);

    fd_table [fd_].events = 0;
    fd_table [fd_].ev_source = ev_source_;
    fd_table [fd_].in_use = true;

    devpoll_ctl (fd_, 0);

    cookie_t cookie;
    cookie.fd = fd_;
    return cookie;
}

void zmq::devpoll_t::rm_fd (cookie_t cookie_)
{
    assert (fd_table [cookie_.fd].in_use);
    devpoll_ctl (cookie_.fd, POLLREMOVE);
    fd_table [cookie_.fd].in_use = false;
}

void zmq::devpoll_t::set_pollin (cookie_t cookie_)
{
    int fd = cookie_.fd;
    fd_table [fd].events |= POLLIN;
    devpoll_ctl (fd, fd_table [fd].events);
}

void zmq::devpoll_t::reset_pollin (cookie_t cookie_)
{
    int fd = cookie_.fd;
    fd_table [fd].events &= ~((short) POLLIN);
    devpoll_ctl (fd, fd_table [fd].events);
}

void zmq::devpoll_t::set_pollout (cookie_t cookie_)
{
    int fd = cookie_.fd;
    fd_table [fd].events |= POLLOUT;
    devpoll_ctl (fd, fd_table [fd].events);
}

void zmq::devpoll_t::reset_pollout (cookie_t cookie_)
{
    int fd = cookie_.fd;
    fd_table [fd].events &= ~((short) POLLOUT);
    devpoll_ctl (fd, fd_table [fd].events);
}

bool zmq::devpoll_t::process_events (poller_t <devpoll_t> *poller_)
{
    struct pollfd ev_buf [max_io_events];
    struct dvpoll poll_req;

    //  According to the poll(7d) manpage, we can retrieve no more then
    //  (OPEN_MAX - 1) events.
    int nfds = max_io_events;
    if (nfds >= OPEN_MAX)
        nfds = OPEN_MAX - 1;

    poll_req.dp_fds = &ev_buf [0];
    poll_req.dp_nfds = nfds;
    poll_req.dp_timeout = -1;

    //  Wait for events.
    int n = ioctl (devpoll_fd, DP_POLL, &poll_req);
    errno_assert (n != -1);

    for (int i = 0; i < n; i ++) {
        int fd = ev_buf [i].fd;
        event_source_t *ev_source = fd_table [fd].ev_source;

        if (ev_buf [i].revents & (POLLERR | POLLHUP))
            if (poller_->process_event (ev_source, ZMQ_EVENT_ERR))
                return true;
        if (ev_buf [i].revents & POLLOUT)
            if (poller_->process_event (ev_source, ZMQ_EVENT_OUT))
                return true;
        if (ev_buf [i].revents & POLLIN)
            if (poller_->process_event (ev_source, ZMQ_EVENT_IN))
                return true;
    }
    return false;
}

#endif
