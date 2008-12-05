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

#if defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_FREEBSD ||\
    defined ZMQ_HAVE_OPENBSD || defined ZMQ_HAVE_SOLARIS ||\
    defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_QNXNTO ||\
    defined ZMQ_HAVE_HPUX

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <poll.h>

#include <zmq/err.hpp>
#include <zmq/poll_thread.hpp>

zmq::poll_t::poll_t () :
    retired (false)
{
    //  Get the limit on open file descriptors. Resize fds so that it
    //  can hold all descriptors.
    rlimit rl;
    int rc = getrlimit (RLIMIT_NOFILE, &rl);
    errno_assert (rc != -1);
    fd_table.resize (rl.rlim_cur);

    for (rlim_t i = 0; i < rl.rlim_cur; i ++)
        fd_table [i].index = -1;
}

zmq::handle_t zmq::poll_t::add_fd (int fd_, i_pollable *engine_)
{
    pollfd pfd = {fd_, 0, 0};
    pollset.push_back (pfd);
    assert (fd_table [fd_].index == -1);

    fd_table [fd_].index = pollset.size() - 1;
    fd_table [fd_].engine = engine_;

    handle_t handle;
    handle.fd = fd_;
    return handle;
}

void zmq::poll_t::rm_fd (handle_t handle_)
{
    int index = fd_table [handle_.fd].index;
    assert (index != -1);

    //  Mark the fd as unused.
    pollset [index].fd = -1;
    fd_table [handle_.fd].index = -1;
    retired = true;
}

void zmq::poll_t::set_pollin (handle_t handle_)
{
    int index = fd_table [handle_.fd].index;
    pollset [index].events |= POLLIN;
}

void zmq::poll_t::reset_pollin (handle_t handle_)
{
    int index = fd_table [handle_.fd].index;
    pollset [index].events &= ~((short) POLLIN);
}

void zmq::poll_t::set_pollout (handle_t handle_)
{
    int index = fd_table [handle_.fd].index;
    pollset [index].events |= POLLOUT;
}

void zmq::poll_t::reset_pollout (handle_t handle_)
{
    int index = fd_table [handle_.fd].index;
    pollset [index].events &= ~((short) POLLOUT);
}

bool zmq::poll_t::process_events (poller_t <poll_t> *poller_)
{
    //  Wait for events.
    int rc = poll (&pollset [0], pollset.size (), -1);
    errno_assert (rc != -1);

    for (pollset_t::size_type i = 0; i < pollset.size (); i ++) {
        assert (!(pollset [i].revents & POLLNVAL));

        int fd = pollset [i].fd;
        i_pollable *engine = fd_table [fd].engine;

        if (pollset [i].fd == -1)
           continue;
        if (pollset [i].revents & (POLLERR | POLLHUP))
            if (poller_->process_event (engine, event_err))
                return true;
        if (pollset [i].fd == -1)
           continue;
        if (pollset [i].revents & POLLOUT)
            if (poller_->process_event (engine, event_out))
                return true;
        if (pollset [i].fd == -1)
           continue;
        if (pollset [i].revents & POLLIN)
            if (poller_->process_event (engine, event_in))
                return true;
    }

    //  Clean up the pollset and update the fd_table accordingly.
    if (retired) {
        pollset_t::size_type i = 0;
        while (i < pollset.size ()) {
            if (pollset [i].fd == -1)
                pollset.erase (pollset.begin () + i);
            else {
                fd_table [pollset [i].fd].index = i;
                i ++;
            }
        }

        retired = false;
    }

    return false;
}

#endif
