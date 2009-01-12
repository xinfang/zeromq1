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

#if defined ZMQ_HAVE_EVENTFD

    class ysocketpair_t : public i_signaler
    {
    public:

        //  Initialise the object.
        inline ysocketpair_t ()
        {
            //  Create eventfd object.
            fd = eventfd (0, 0);
            errno_assert (fd != -1);

            //  Set to non-blocking mode.
            int flags = fcntl (fd, F_GETFL, 0);
            if (flags == -1)
                flags = 0;
            int rc = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
            errno_assert (rc != -1);
        }

        //  Destroy the object.
        inline ~ysocketpair_t ()
        {
            int rc = close (fd);
            errno_assert (rc != -1);
        }

        //  Send specific signal.
        void signal (int signal_)
        {
            assert (signal_ >= 0 && signal_ < 32);
            uint64_t inc = 1;
            inc <<= signal_;
            ssize_t sz = write (fd, &inc, sizeof (uint64_t));
            errno_assert (sz == sizeof (uint64_t));
        }

        //  Retrieves signals. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc. If there is no signal available,
        //  it returns zero immediately.
        inline uint32_t check ()
        {
            uint64_t val;
            ssize_t sz = read (fd, &val, sizeof (uint64_t));
            if (sz == -1 && errno == EAGAIN)
                return 0;
            errno_assert (sz != -1);
            return (uint32_t) val;
        }

        //  Get the file descriptor associated with the object.
        inline fd_t get_fd ()
        {
            return fd;
        }

    private:

        //  eventfd descriptor.
        int fd;

        //  Disable copying of ysocketpair object.
        ysocketpair_t (const ysocketpair_t&);
        void operator = (const ysocketpair_t&);
    };

#elif defined ZMQ_HAVE_WINDOWS

    class ysocketpair_t : public i_signaler
    {
    public:

        //  Initialise the pipe.
        ZMQ_EXPORT ysocketpair_t ();

        //  Destroy the pipe.
        ZMQ_EXPORT ~ysocketpair_t ();

        //  Send specific signal to the pipe (i_signaler implemenation).
        ZMQ_EXPORT void signal (int signal_);

        //  Waits for a signal. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc.
        ZMQ_EXPORT inline uint32_t poll ()
        {
            DWORD rc = WaitForSingleObject ((HANDLE) r, INFINITE);
            assert (rc != WAIT_ABANDONED);
            assert (rc != WAIT_TIMEOUT);
            win_assert (rc != WAIT_FAILED);
            return check ();
        }

        //  Retrieves signals. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc. If there is no signal available,
        //  it returns zero immediately.
        ZMQ_EXPORT inline uint32_t check ()
        {
            char buffer [256];
          
            int nbytes = recv (r, buffer, 256, 0);
            win_assert (nbytes != -1);

            uint32_t signals = 0;
            for (int pos = 0; pos != nbytes; pos ++) {
                assert (buffer [pos] < 31);
                signals |= (1 << (buffer [pos]));
            }
            return signals;
        }

        //  Get the file descriptor associated with the pipe.
        ZMQ_EXPORT inline fd_t get_fd ()
        {
            return r;
        }

    private:

        //  Write end of the socketpair.
        fd_t w;
        
        //  Read end of the socketpair.
        fd_t r;

        //  Disable copying of ysocketpair object.
        ysocketpair_t (const ysocketpair_t&);
        void operator = (const ysocketpair_t&);
    };

#else

    class ysocketpair_t : public i_signaler
    {
    public:

        //  Initialise the pipe.
        inline ysocketpair_t ()
        {
            int sv [2];
            int rc = socketpair (AF_UNIX, SOCK_STREAM, 0, sv);
            errno_assert (rc == 0);
            w = sv [0];
            r = sv [1];
                      
            //  Set to non-blocking mode.
            int flags = fcntl (r, F_GETFL, 0);
            if (flags == -1)
                flags = 0;
            rc = fcntl (r, F_SETFL, flags | O_NONBLOCK);
            errno_assert (rc != -1);
        }

        //  Destroy the pipe.
        inline ~ysocketpair_t ()
        {
            close (w);
            close (r);
        }

        //  Send specific signal to the pipe.
        void signal (int signal_);

        //  Waits for a signal. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc.
        inline uint32_t poll ()
        {
            pollfd pfd = {r, POLLIN, 0};
            int rc = ::poll (&pfd, 1, -1);
            errno_assert (rc >= 0);
            assert (pfd.revents & POLLIN);
            return check ();
        }

        //  Retrieves signals. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc. If there is no signal available,
        //  it returns zero immediately.
        inline uint32_t check ()
        {
            unsigned char buffer [256];
            ssize_t nbytes = recv (r, buffer, 256, 0);
            errno_assert (nbytes != -1);
            uint32_t signals = 0;
            for (int pos = 0; pos != nbytes; pos ++) {
                assert (buffer [pos] < 31);
                signals |= (1 << (buffer [pos]));
            }
            return signals;
        }

        //  Get the file descriptor associated with the pipe.
        inline fd_t get_fd ()
        {
            return r;
        }

    private:

        //  Write end of the socketpair.
        fd_t w;

        //  Read end of the socketpair.
        fd_t r;

        //  Disable copying of ysocketpair object.
        ysocketpair_t (const ysocketpair_t&);
        void operator = (const ysocketpair_t&);
    };

#endif
}

#endif
