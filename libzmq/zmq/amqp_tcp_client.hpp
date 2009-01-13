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

#ifndef __ZMQ_AMQP_TCP_CLIENT_HPP_INCLUDED__
#define __ZMQ_AMQP_TCP_CLIENT_HPP_INCLUDED__

#if defined ZMQ_HAVE_AMQP

#include <zmq/export.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/mux.hpp>
#include <zmq/demux.hpp>
#include <zmq/tcp_socket.hpp>

namespace zmq
{

    class amqp_tcp_client_t : public i_pollable
    {
        //  Allow class factory to create this engine.
        friend class pollable_factory_t;

    public:

        //  i_pollable interface implementation.
        engine_type_t type ();
        void get_watermarks (uint64_t *hwm_, uint64_t *lwm_);
        void process_command (const engine_command_t &command_);
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void unregister_event ();

    private:

        amqp_tcp_client_t (i_thread *calling_thread_, i_thread *thread_,
            const char *hostname_, const char *arguments_);
        ~amqp_tcp_client_t ();

        //  Object to aggregate messages from inbound pipes.
        mux_t mux;

        //  Object to distribute messages to outbound pipes.
        demux_t demux;

        //  AMQP/TCP socket connected to the broker.
        tcp_socket_t socket;
    };

}

#endif

#endif
