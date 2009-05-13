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

#include <zmq/out_engine.hpp>
#include <zmq/err.hpp>

zmq::out_engine_t *zmq::out_engine_t::create (i_demux *demux_)
{
    out_engine_t *instance = new out_engine_t (demux_);
    zmq_assert (instance);
    return instance;
}

zmq::out_engine_t::out_engine_t (i_demux *demux_) :
    demux (demux_)
{
    zmq_assert (demux);
}

zmq::out_engine_t::~out_engine_t ()
{
    delete demux;
}

bool zmq::out_engine_t::write (message_t &msg_)
{
    return demux->write (msg_);
}

void zmq::out_engine_t::flush ()
{
    demux->flush ();
}

void zmq::out_engine_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = 0;
    *lwm_ = 0;
}

int64_t zmq::out_engine_t::get_swap_size ()
{
    return 0;
}

zmq::i_demux *zmq::out_engine_t::get_demux ()
{
    return demux;
}

zmq::i_mux *zmq::out_engine_t::get_mux ()
{
    zmq_assert (false);
    return NULL;
}

zmq::i_pollable *zmq::out_engine_t::cast_to_pollable ()
{
    zmq_assert (false);
    return NULL;
}

const char *zmq::out_engine_t::get_arguments ()
{
    zmq_assert (false);
    return NULL;
}

void zmq::out_engine_t::revive (pipe_t *pipe_)
{
    zmq_assert (false);
}

void zmq::out_engine_t::head (pipe_t *pipe_, int64_t position_)
{
}

void zmq::out_engine_t::send_to (pipe_t *pipe_)
{
    //  Start sending messages to a pipe.
    demux->send_to (pipe_);
}

void zmq::out_engine_t::receive_from (pipe_t *pipe_)
{
    zmq_assert (false);
}
