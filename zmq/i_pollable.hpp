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

#ifndef __ZMQ_I_POLLABLE_HPP_INCLUDED__
#define __ZMQ_I_POLLABLE_HPP_INCLUDED__

#include "i_engine.hpp"
#include "i_poller.hpp"

namespace zmq
{
    //  Virtual interface to be exposed by engines for communication with
    //  file-descriptor-oriented I/O threads.

    struct i_pollable : public i_engine
    {
        //  The destructor shouldn't be virtual, however, not defining it as
        //  such results in compiler warnings with some compilers.
        declspec_export virtual ~i_pollable () {};

        //  Called by I/O thread when engine is being registered
        //  with the thread.
        declspec_export virtual void register_event (i_poller *poller_) = 0;

        //  Called by I/O thread when file descriptor is ready for reading.
        declspec_export virtual void in_event () = 0;

        //  Called by I/O thread when file descriptor is ready for writing.
        declspec_export virtual void out_event () = 0;

        //  Called by I/O thread when error occurs on the file descriptor.
        declspec_export virtual void error_event () = 0;

        //  Called by poll thread when unregistering the engine.
        declspec_export virtual void unregister_event () = 0;

    };

}

#endif
