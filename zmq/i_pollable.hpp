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

#ifndef __ZMQ_I_POLLABLE_HPP_INCLUDED__
#define __ZMQ_I_POLLABLE_HPP_INCLUDED__

#include "i_signaler.hpp"

namespace zmq
{

    //  Virtual interface to be exposed by engines for communication with
    //  poll thread.
    struct i_pollable
    {
        //  The destructor shouldn't be virtual, however, not defining it as
        //  such results in compiler warnings with some compilers.
        virtual ~i_pollable () {};

        //  Attach the engine with the dispatcher
        virtual void set_signaler (i_signaler *signaler_) = 0;

        //  Notifies the engine that another engine sent it messages
        virtual void revive (pollfd *pfd_, int count_, int engine_id_) = 0;

        //  Fills file descriptors array to be used by poll thread to poll on
        //  Returns number of filled fds
//        virtual int get_fd (int *fds_, int nfds_) = 0;

        // Returns number of file descriptors to be used with engine
        virtual int get_fd_count () = 0;

        //  Fills pollfd structures to be used by poll thread to poll on
        virtual int get_pfds (pollfd *fds_, int count_) = 0;

        //  Returns events poll thread should poll for
//        virtual short get_events () = 0;

        //  Called by poll thread when in event occurs
        virtual void in_event (pollfd *pfd_, int count_, int index_) = 0;

        //  Called by poll thread when out event occurs
        virtual void out_event (pollfd *pfd_, int count_, int index_) = 0;
    };
}
//  Returns number of file descriptors to be used
//          virtual int get_fd_count () = 0;
//
//  Returns file descriptor to be used by poll thread to poll on
//          virtual int get_pfds (pollfd *fds_, int count_) = 0;
//
//  Called by poll thread when in event occurs
//          virtual void in_event (pollfd *pfd_, int count_, int index_) = 0;
//
//  Called by poll thread when out event occurs
//          virtual void out_event (pollfd *pfd_, int count_, int index_) = 0;
#endif
