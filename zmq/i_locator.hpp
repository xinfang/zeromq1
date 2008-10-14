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

#ifndef __ZMQ_I_LOCATOR_HPP_INCLUDED__
#define __ZMQ_I_LOCATOR_HPP_INCLUDED__

#include "i_thread.hpp"
#include "i_engine.hpp"
#include "scope.hpp"
#include "zmq_server.hpp"

namespace zmq
{

    

    struct i_locator
    {
        //  The destructor shouldn't be virtual, however, not defining it as
        //  such results in compiler warnings with some compilers.
        virtual ~i_locator () {};

        //  Creates an object.
        virtual void create (i_thread *calling_thread_,
            unsigned char type_id_, const char *object_,
            i_thread *thread_, i_engine *engine_, scope_t scope_,
            const char *interface_,
            i_thread *listener_thread_, int handler_thread_count_,
            i_thread **handler_threads_) = 0;

        //  Gets the engine that handles specified object.
        //  Returns false if the object is not known.
        virtual bool get (i_thread *calling_thread_,
            unsigned char type_id_, const char *object_,
            i_thread **thread_, i_engine **engine_,
            class i_thread *thread_, const char *local_object_) = 0;
    };

}

#endif

