/*
    Copyright (c) 2007-2009 FastMQ Inc.

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

#ifndef __ZMQ_YSOCKETPAIR_HPP_INCLUDED__
#define __ZMQ_YSOCKETPAIR_HPP_INCLUDED__

#include <assert.h>

#include <zmq/platform.hpp>
#include <zmq/stdint.hpp>
#include <zmq/i_signaler.hpp>
#include <zmq/err.hpp>
#include <zmq/tcp_socket.hpp>
#include <zmq/tcp_listener.hpp>
#include <zmq/formatting.hpp>
#include <zmq/ip.hpp>
#include <zmq/fd.hpp>
#include <zmq/export.hpp>

#ifdef ZMQ_HAVE_WINDOWS 
#include <zmq/windows.hpp>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <fcntl.h>
#endif

#if defined ZMQ_HAVE_EVENTFD
#include <sys/eventfd.h>
#endif

namespace zmq
{

    //  This object can be used to send individual signals from one thread to
    //  another. The specific of this pipe is that it has associated file
    //  descriptor and so it can be polled on. Same signal cannot be sent twice
    //  unless signals are retrieved by the reader side in the meantime.

    class ysocketpair_t : public i_signaler
    {
    public:

        //  Initialise the object.
        ZMQ_EXPORT ysocketpair_t ();

        //  Destroy the object.
        ZMQ_EXPORT ~ysocketpair_t ();

        //  Send specific signal.
        void signal (int signal_);

        //  Retrieves signals. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc. If there is no signal available,
        //  it returns zero immediately.
        uint32_t check ();

        //  Get the file descriptor associated with the object.
        ZMQ_EXPORT fd_t get_fd ();

    private:

#if defined ZMQ_HAVE_EVENTFD
        //  eventfd descriptor.
        fd_t fd;
#else
        //  Write & read end of the socketpair.
        fd_t w;
        fd_t r;
#endif

        //  Disable copying of ysocketpair object.
        ysocketpair_t (const ysocketpair_t&);
        void operator = (const ysocketpair_t&);
    };

}

#endif
