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

#include <zmq/in_engine.hpp>

zmq::in_engine_t *zmq::in_engine_t::create (int hwm_, int lwm_)
{
    in_engine_t *instance = new in_engine_t (hwm_, lwm_);
    assert (instance);
    return instance;
}

zmq::in_engine_t::in_engine_t (int hwm_, int lwm_) :
    hwm (hwm_),
    lwm (lwm_)
{
}

zmq::in_engine_t::~in_engine_t ()
{
}

bool zmq::in_engine_t::read (message_t *msg_)
{
    return mux.read (msg_);
}

zmq::engine_type_t zmq::in_engine_t::type ()
{
    return engine_type_api;
}

void zmq::in_engine_t::get_watermarks (int *hwm_, int *lwm_)
{
    *hwm_ = hwm;
    *lwm_ = lwm;
}

void zmq::in_engine_t::process_command (const engine_command_t &command_)
{
    switch (command_.type) {
    case engine_command_t::revive:

        //  Forward the revive command to the pipe.
        command_.args.revive.pipe->revive ();
        break;

    case engine_command_t::receive_from:

        //  Start receiving messages from a pipe.
        mux.receive_from (command_.args.receive_from.pipe, false);
        break;

    case engine_command_t::terminate_pipe_ack:

        //  Forward the command to the pipe. Drop reference to the pipe.
        command_.args.terminate_pipe.pipe->reader_terminated ();
        mux.release_pipe (command_.args.terminate_pipe.pipe);
        break;

    default:

        //  Unsupported/unknown command.
        assert (false);
     }
}
