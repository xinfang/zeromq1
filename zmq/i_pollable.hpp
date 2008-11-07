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

#include <poll.h>

#include "i_engine.hpp"
#include "i_poller.hpp"

namespace zmq
{
    //  Virtual interface to be exposed by engines for communication with
    //  poll thread.
    struct i_pollable : public i_engine
    {
        //  The destructor shouldn't be virtual, however, not defining it as
        //  such results in compiler warnings with some compilers.
        virtual ~i_pollable () {};

        //  Set poller & poll handle associated with this engine.
        virtual void set_poller (i_poller *poller_, int handle_) = 0;

        //  Called by poll thread when in event occurs.
        virtual bool in_event () = 0;

        //  Called by poll thread when out event occurs.
        virtual bool out_event () = 0;

        //  Called by poll thread when close event occurs. If function returns
        //  false, thread handling the engine should unregister it.
        virtual bool close_event () = 0;
    };

}

#endif
