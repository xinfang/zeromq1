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

#ifndef __ZMQ_I_ENGINE_HPP_INCLUDED__
#define __ZMQ_I_ENGINE_HPP_INCLUDED__

#include "export.hpp"
#include "command.hpp"

namespace zmq
{

    enum engine_type_t
    {
        //  Engines of this type expose i_pollable interface.
        engine_type_fd,

        //  Exposed by api_thread_t.
        engine_type_api
    };

    //  Virtual interface to be exposed by engines so that they can receive
    //  commands from other engines.
    struct i_engine
    {
        ZMQ_EXPORT virtual ~i_engine () {};

        //  Returns type of the engine. This can be in theory implemented on
        //  the base of standard C++ RTTI, however, it's unclear whether RTTI
        //  is supported by all compilers and if so, what's the impact of RTTI
        //  performance on overall performance of the product.
        virtual engine_type_t type () = 0;

        //  Called when command from a different thread is received.
        virtual void process_command (
            const struct engine_command_t &command_) = 0;
    };

}

#endif
