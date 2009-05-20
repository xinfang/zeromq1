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

#ifndef __ZMQ_I_ENGINE_HPP_INCLUDED__
#define __ZMQ_I_ENGINE_HPP_INCLUDED__

#include <zmq/stdint.hpp>
#include <zmq/command.hpp>

namespace zmq
{

    //  Virtual interface to be exposed by engines so that they can receive
    //  commands from other engines.
    struct i_engine
    {
        virtual ~i_engine () {};

        //  Put the engine into operation.
        virtual void start (struct i_thread *current_thread_,
            struct i_thread *engine_thread_) = 0;

        //  Returns the engine's demux or fails if the engine doesn't
        //  use one. This is temporary function. It should pass away before
        //  the core refactoring is done.
        virtual class i_demux *get_demux () = 0;

        //  Returns the engine's mux or fails if the engine doesn't
        //  use one. This is a temporary function. It should pass away before
        //  the core refactoring is done.
        virtual class i_mux *get_mux () = 0;

        //  Returns modified arguments string.
        //  This function will be obsoleted with the shift to centralised
        //  management of configuration.
        virtual const char *get_arguments () = 0;
    };

}

#endif
