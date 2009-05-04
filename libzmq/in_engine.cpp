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

#include <zmq/in_engine.hpp>
#include <zmq/err.hpp>
#include <zmq/i_engine.hpp>
#include <zmq/mux.hpp>

zmq::in_engine_t *zmq::in_engine_t::create (mux_t *mux_, int64_t hwm_, 
    int64_t lwm_, uint64_t swap_size_)
{
    in_engine_t *instance = new in_engine_t (mux_, hwm_, lwm_, swap_size_);
    zmq_assert (instance);
    return instance;
}

zmq::in_engine_t::in_engine_t (mux_t *mux_, int64_t hwm_, int64_t lwm_,
      int64_t swap_size_) :
    mux (mux_),
    hwm (hwm_),
    lwm (lwm_),
    swap_size (swap_size_)
{
    zmq_assert (mux);
}

zmq::in_engine_t::~in_engine_t ()
{
    delete mux;
}

bool zmq::in_engine_t::read (message_t *msg_)
{
    return mux->read (msg_);
}

void zmq::in_engine_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = hwm;
    *lwm_ = lwm;
}

int64_t zmq::in_engine_t::get_swap_size ()
{
    return swap_size;
}

zmq::i_pollable *zmq::in_engine_t::cast_to_pollable ()
{
    zmq_assert (false);
    return NULL;
}

const char *zmq::in_engine_t::get_arguments ()
{
    zmq_assert (false);
    return NULL;
}

void zmq::in_engine_t::revive (pipe_t *pipe_)
{
    //  Notify the reader of the pipe that there are messages
    //  available in the pipe.
    mux->revive (pipe_);
}

void zmq::in_engine_t::head (pipe_t *pipe_, int64_t position_)
{
    zmq_assert (false);
}

void zmq::in_engine_t::send_to (pipe_t *pipe_)
{
    zmq_assert (false);
}

void zmq::in_engine_t::receive_from (pipe_t *pipe_)
{
    //  Start receiving messages from a pipe.
    mux->receive_from (pipe_);
}

void zmq::in_engine_t::terminate_pipe (pipe_t *pipe_)
{
    zmq_assert (false);
}

void zmq::in_engine_t::terminate_pipe_ack (pipe_t *pipe_)
{
    //  Drop reference to the pipe.
    mux->release_pipe (pipe_);
}

