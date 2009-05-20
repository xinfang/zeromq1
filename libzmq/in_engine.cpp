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

zmq::in_engine_t *zmq::in_engine_t::create (mux_t *mux_)
{
    in_engine_t *instance = new in_engine_t (mux_);
    zmq_assert (instance);
    return instance;
}

zmq::in_engine_t::in_engine_t (mux_t *mux_) :
    mux (mux_)
{
    zmq_assert (mux);
}

zmq::in_engine_t::~in_engine_t ()
{
    delete mux;
}

void zmq::in_engine_t::start (i_thread *, i_thread *)
{
    mux->register_engine (this);
}

bool zmq::in_engine_t::read (message_t *msg_)
{
    return mux->read (msg_);
}

zmq::i_demux *zmq::in_engine_t::get_demux ()
{
    zmq_assert (false);
    return NULL;
}

zmq::i_mux *zmq::in_engine_t::get_mux ()
{
    return mux;
}

const char *zmq::in_engine_t::get_arguments ()
{
    zmq_assert (false);
    return NULL;
}

void zmq::in_engine_t::revive ()
{
}

void zmq::in_engine_t::head ()
{
    zmq_assert (false);
}

void zmq::in_engine_t::send_to ()
{
    zmq_assert (false);
}

void zmq::in_engine_t::receive_from ()
{
}
