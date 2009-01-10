/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_AMQP_ENCODER_HPP_INCLUDED__
#define __ZMQ_AMQP_ENCODER_HPP_INCLUDED__

#if defined ZMQ_HAVE_AMQP

#include <string>

#include <zmq/encoder.hpp>

namespace zmq
{

    //  Encoder for AMQP.
    class amqp_encoder_t : public encoder_t <amqp_encoder_t>
    {
    public:

        //  Create the encoder. Specifies dispatcher proxy and marshaller
        //  to use and id of engine to receive messages from. 'server' parameter
        //  specifies whether messages are sent using 'basic.deliver' command
        //  (server) or 'basic.publish' command (client).
        amqp_encoder_t (dispatcher_proxy_t *proxy_, int source_engine_id_,
            amqp_marshaller_t *marshaller_, bool server_,
            const char *out_exchange_, const char *out_routing_key_);
        ~amqp_encoder_t ();

    private:

        bool message_ready ();
        bool command_header ();
        bool command_arguments ();
        bool content_header ();
        bool content_body_frame_header ();
        bool content_body ();
        bool frame_end ();

        bool server;

        unsigned char *tmpbuf;
        size_t tmpbuf_size;

        amqp_marshaller_t::command_t command;
        cmsg_t message;
        size_t body_offset;

        dispatcher_proxy_t *proxy;
        int source_engine_id;
        amqp_marshaller_t *marshaller;

        bool flow_on;

        std::string out_exchange;
        std::string out_routing_key;
    };

}

#endif

#endif

