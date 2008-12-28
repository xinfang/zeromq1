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

#include <zmq/out_engine.hpp>

zmq::out_engine_t *zmq::out_engine_t::create ()
{
    out_engine_t *instance = new out_engine_t ();
    assert (instance);
    return instance;
}

zmq::out_engine_t::out_engine_t ()
{
}

zmq::out_engine_t::~out_engine_t ()
{
}

bool zmq::out_engine_t::write (message_t &msg_)
{
    return demux.write (msg_);
}

void zmq::out_engine_t::flush ()
{
    demux.flush ();
}

zmq::engine_type_t zmq::out_engine_t::type ()
{
    return engine_type_api;
}

void zmq::out_engine_t::get_watermarks (int *hwm_, int *lwm_)
{
    *hwm_ = 0;
    *lwm_ = 0;
}

void zmq::out_engine_t::process_command (const engine_command_t &command_)
{
    switch (command_.type) {

    case engine_command_t::head:

       //  Forward pipe head position to the appropriate pipe.
       command_.args.head.pipe->set_head (command_.args.head.position);
       break;

    case engine_command_t::send_to:

        //  Start sending messages to a pipe.
        demux.send_to (command_.args.send_to.pipe);
        break;

    case engine_command_t::terminate_pipe:

        //  Forward the command to the pipe. Drop reference to the pipe.
        command_.args.terminate_pipe.pipe->writer_terminated ();
        demux.release_pipe (command_.args.terminate_pipe.pipe);
        break;

    default:

        //  Unsupported/unknown command.
        assert (false);
     }
}
