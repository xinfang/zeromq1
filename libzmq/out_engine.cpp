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

zmq::out_engine_t *zmq::out_engine_t::create (bool load_balancing_)
{
    out_engine_t *instance = new out_engine_t (load_balancing_);
    assert (instance);
    return instance;
}

zmq::out_engine_t::out_engine_t (bool load_balancing_) :
    engine_base_t <true, false> (load_balancing_)
{
}

zmq::out_engine_t::~out_engine_t ()
{
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
