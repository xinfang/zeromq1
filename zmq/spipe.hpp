/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __ZMQ_SPIPE_HPP_INCLUDED__
#define __ZMQ_SPIPE_HPP_INCLUDED__

#include <unistd.h>
#include <sys/socket.h>

#include "err.hpp"

namespace zmq
{

    //  This pipe can be used to send individual bytes from one thread to
    //  another. The specific of this queue is that it has associated file
    //  descriptor and it can be polled for.
    class spipe_t
    {
    public:

        //  Initialise the pipe
        inline spipe_t ()
        {
            int sv [2];
            int rc = socketpair (AF_UNIX, SOCK_STREAM, 0, sv);
            errno_assert (rc == 0);
            w = sv [0];
            r = sv [1];
        }

        //  Destroy the pipe
        inline ~spipe_t ()
        {
            close (w);
            close (r);
        }

        //  Send one byte to the pipe
        inline void write (unsigned char c)
        {
            ssize_t nbytes = send (w, &c, 1, 0);
            errno_assert (nbytes == 1);
        }

        //  Read one byte from the pipe
        inline unsigned char read ()
        {
            unsigned char c;
            ssize_t nbytes = recv (r, &c, 1, MSG_WAITALL);
            errno_assert (nbytes == 1);
            return c;
        }

        //  Get the file descriptor associated with the pipe
        inline operator int ()
        {
            return r;
        }

    protected:
        int w;
        int r;
    };

}

#endif
