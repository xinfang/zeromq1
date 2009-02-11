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

        //  Returns i_pollable interface of the engine. If the engine is not
        //  pollable, it fails.
        virtual struct i_pollable *cast_to_pollable () = 0;

        //  Returns high and low watermarks for the specified engine. High and
        //  low watermarks for a pipe are computed by adding high and low
        //  watermarks of the engines the pipe is connecting. hwm equal to -1
        //  means that there should be unlimited storage space for the engine.
        virtual void get_watermarks (int64_t *hwm_, int64_t *lwm_) = 0;

        //  Inter-thread commands.
        virtual void revive (class pipe_t *pipe_) = 0;
        virtual void head (class pipe_t *pipe_, int64_t position_) = 0;
        virtual void send_to (class pipe_t *pipe_) = 0;
        virtual void receive_from (class pipe_t *pipe_) = 0;
        virtual void terminate_pipe (class pipe_t *pipe_) = 0;
        virtual void terminate_pipe_ack (class pipe_t *pipe_) = 0;
    };

}

#endif
