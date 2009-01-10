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

#if defined ZMQ_HAVE_AMQP

#include <zmq/amqp_tcp_client.hpp>
#include <zmq/config.hpp>

zmq::amqp_tcp_client_t *zmq::amqp_tcp_client_t::create (
    i_thread *calling_thread_, i_thread *thread_, const char *hostname_)
{
    amqp_tcp_client_t *instance = new amqp_tcp_client_t (calling_thread_,
        thread_, hostname_);
    assert (instance);

    return instance;
}

zmq::amqp_tcp_client_t::amqp_tcp_client_t (i_thread *calling_thread_,
    i_thread *thread_, const char *hostname_)
{
    assert (false);
}

zmq::amqp_tcp_client_t::~amqp_tcp_client_t ()
{
}

zmq::engine_type_t zmq::amqp_tcp_client_t::type ()
{
    return engine_type_fd;
}

void zmq::amqp_tcp_client_t::get_watermarks (uint64_t *hwm_, uint64_t *lwm_)
{
    //  TODO: Rename bp_hwm & bp_lwm to generic "connection_hwm" &
    //  "connection_lwm" it is not tied strictly to the backend protocol.
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

void zmq::amqp_tcp_client_t::process_command (const engine_command_t &command_)
{
    assert (false);
}

void zmq::amqp_tcp_client_t::register_event (i_poller *poller_)
{
    assert (false);
}

void zmq::amqp_tcp_client_t::in_event ()
{
    assert (false);
}

void zmq::amqp_tcp_client_t::out_event ()
{
    assert (false);
}

void zmq::amqp_tcp_client_t::unregister_event ()
{
    assert (false);
}

#endif
