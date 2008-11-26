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

#ifndef __ZMQ_DEVPOLL_THREAD_HPP_INCLUDED__
#define __ZMQ_DEVPOLL_THREAD_HPP_INCLUDED__

#include <zmq/platform.hpp>

#ifdef ZMQ_HAVE_SOLARIS

#include <vector>

#include <zmq/poller.hpp>

namespace zmq
{

    //  Implements socket polling mechanism using the Solaris-specific
    //  "/dev/poll" interface. The class is used when instantiating the poller
    //  template to generate the devpoll_thread_t class.

    class devpoll_t
    {
    public:

        devpoll_t ();
        ~devpoll_t ();

        cookie_t add_fd (int fd_, event_source_t *ev_source_);
        void rm_fd (cookie_t handle_);
        void set_pollin (cookie_t handle_);
        void reset_pollin (cookie_t handle_);
        void set_pollout (cookie_t handle_);
        void reset_pollout (cookie_t handle_);
        void wait (event_list_t &event_list_);

    private:

        //  File descriptor referring to "/dev/poll" pseudo-device.
        int devpoll_fd;

        struct fd_entry {
            short events;
            event_source_t *ev_source;
            bool in_use;
        };

        std::vector <fd_entry> fd_table;

        //  Pollset manipulation function.
        void devpoll_ctl (int fd_, short events_);

        devpoll_t (const devpoll_t&);
        void operator = (const devpoll_t&);
    };

    typedef poller_t <devpoll_t> devpoll_thread_t;

}

#endif

#endif
