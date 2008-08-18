/*
    Copyright (c) 2007-2008 FastMQ Inc.

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

#ifndef __ZMQ_I_POLLER_HPP_INCLUDED__
#define __ZMQ_I_POLLER_HPP_INCLUDED__

namespace zmq
{
    //  Virtual interface to be exposed by engines for communication with
    //  poll thread.
    struct i_poller
    {
        //  The destructor shouldn't be virtual, however, not defining it as
        //  such results in compiler warnings with some compilers.
        virtual ~i_poller () {};

        //  Specify the file descriptor to use for polling.
        virtual void set_fd (int handle_, int fd_) = 0;

        //  Start polling for input from socket.
        virtual void set_pollin (int handle_) = 0;

        //  Stop polling for input from socket.
        virtual void reset_pollin (int handle_) = 0;

        //  Try to read from socket (even though we are not sure there's
        //  anything to read there).
        virtual void speculative_read (int handle_) = 0;

        //  Start polling for availability of the socket for writing.
        virtual void set_pollout (int handle_) = 0;

        //  Stop polling for availability of the socket for writing.
        virtual void reset_pollout (int handle_) = 0;

        //  Try to write to the socket (even though we are not sure the
        //  write will succeed).
        virtual void speculative_write (int handle_) = 0;
    };

}

#endif
