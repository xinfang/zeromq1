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

#ifndef __ZMQ_AMQP_ENCODER_HPP_INCLUDED__
#define __ZMQ_AMQP_ENCODER_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <string>

#include <zmq/i_amqp.hpp>
#include <zmq/encoder.hpp>
#include <zmq/amqp_marshaller.hpp>
#include <zmq/mux.hpp>
#include <zmq/message.hpp>

namespace zmq
{

    //  Encoder for AMQP.
    class amqp_encoder_t :
        public encoder_t <amqp_encoder_t>,
        public amqp_marshaller_t
    {
    public:

        //  Create the encoder.
        amqp_encoder_t (mux_t *mux_, const char *queue_);
        ~amqp_encoder_t ();

        //  Switch message flow on/off on a particular channel.
        void flow (bool flow_on_, uint16_t channel_);

    private:

        bool message_ready ();
        bool command_header ();
        bool command_arguments ();
        bool content_header ();
        bool content_body_frame_header ();
        bool content_body ();
        bool frame_end ();

        //  Mux object to get data from.
        mux_t *mux;

        //  AMQP command currently being encoded.
        command_t command;

        //  Message currently being encoded. message_offset points to
        //  the beginning of yet un-encoded part of the message.
        message_t message;
        size_t message_offset;

        //  Queue to send/receiver messages from.
        std::string queue;

        //  If true, messages may pass through the encoder. If false, only
        //  AMQP commands are passed (e.g. during initial AMQP handshaking).
        bool flow_on;

        //  AMQP channel to send messages on.
        uint16_t message_channel;

        //  Buffer used to compose the frames (excluding actual
        //  message payload).
        unsigned char framebuf [i_amqp::frame_min_size];
        enum {framebuf_size = i_amqp::frame_min_size};

        amqp_encoder_t (const amqp_encoder_t&);
        void operator = (const amqp_encoder_t&);
    };

}

#endif

#endif

