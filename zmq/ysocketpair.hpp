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

#include "platform.hpp"
#include "declspec_export.hpp"

#ifndef ZMQ_HAVE_WINDOWS 
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#endif

#if defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_FREEBSD || defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_WINDOWS

#include <assert.h>
#include "stdint.hpp"
#include "i_signaler.hpp"
#include "err.hpp"
#endif

namespace zmq
{

#ifdef ZMQ_HAVE_WINDOWS
    //  This pipe can be used to send individual bytes from one thread to
    //  another. The specific of this pipe is that it has associated file
    //  descriptor and so it can be polled on.

    class ysocketpair_t : public i_signaler
    {
    public:

        //  Initialise the pipe.
        declspec_export inline ysocketpair_t ()
        {
            
            
            int rc = CreatePipe ( &r, &w, NULL, 0);
            errno_assert (rc != 0);
            
        }

        //  Destroy the pipe.
        declspec_export inline ~ysocketpair_t ()
        {
//            close (w);
//            close (r);
        }

        //  Send specific signal to the pipe.
        declspec_export void signal (int signal_);

        //  Waits for a signal. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc.
        declspec_export inline uint32_t poll ()
        {
            int rc = WaitForSingleObject(r, INFINITE);          
            errno_assert (rc >= 0);
            //  TODO find out, if r is ready to read from
            return check ();
        }

        //  Retrieves signals. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc. If there is no signal available,
        //  it returns zero immediately.
        declspec_export inline uint32_t check ()
        {
            unsigned char buffer [256];
           
            int nbytes = ReadFile (r, buffer, 256, NULL, NULL);
            errno_assert (nbytes != -1);
            uint32_t signals = 0;
            for (int pos = 0; pos != nbytes; pos ++) {
                assert (buffer [pos] < 31);
                signals |= (1 << (buffer [pos]));
            }
            return signals;
        }

        //  Get the file descriptor associated with the pipe.
        declspec_export inline int get_fd ()
        {
            return (int) r;
        }

    private:

        //  Write end of the socketpair.
        HANDLE w;
        
        //  Read end of the socketpair.
        HANDLE r;

        //  Disable copying of ysocketpair object.
        ysocketpair_t (const ysocketpair_t&);
        void operator = (const ysocketpair_t&);
    };
#else

    //  This pipe can be used to send individual bytes from one thread to
    //  another. The specific of this pipe is that it has associated file
    //  descriptor and so it can be polled on.

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
            ssize_t nbytes = recv (r, buffer, 256, MSG_DONTWAIT);
            errno_assert (nbytes != -1);
            uint32_t signals = 0;
            for (int pos = 0; pos != nbytes; pos ++) {
                assert (buffer [pos] < 31);
                signals |= (1 << (buffer [pos]));
            }
            return signals;
        }

        //  Get the file descriptor associated with the pipe.
        inline int get_fd ()
        {
            return r;
        }

    private:

        //  Write end of the socketpair.
        int w;

        //  Read end of the socketpair.
        int r;

        //  Disable copying of ysocketpair object.
        ysocketpair_t (const ysocketpair_t&);
        void operator = (const ysocketpair_t&);
    };

#endif
}

#endif
