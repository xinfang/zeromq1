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

#ifndef __ZMQ_AMQP_DECODER_HPP_INCLUDED__
#define __ZMQ_AMQP_DECODER_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <zmq/i_amqp.hpp>
#include <zmq/decoder.hpp>
#include <zmq/amqp_unmarshaller.hpp>
#include <zmq/i_demux.hpp>
#include <zmq/message.hpp>

namespace zmq
{

    class amqp_decoder_t :
        public decoder_t <amqp_decoder_t>,
        public amqp_unmarshaller_t
    {
    public:

        amqp_decoder_t (i_demux *demux_, i_amqp *callback_);
        ~amqp_decoder_t ();

        //  Switch message flow on/off.
        void flow (bool flow_on_, uint16_t channel_);

        //  Clean up any half-read commands/messages.
        void reset ();

    private:

        bool method_frame_header_ready ();
        bool method_payload_ready ();
        bool content_header_frame_header_ready ();
        bool content_header_payload_ready ();
        bool content_body_frame_header_ready ();
        bool content_body_payload_ready ();
        bool content_body_frame_end_ready ();

        //  Object to push decoded messages to.
        i_demux *demux;

        //  This variable is used to inform next step of the state machine
        //  how much meaningful data was actually read.
        //  TODO: This should also include frame-end octet, which is not
        //        the case now.
        //  TODO: Shouldn't it be moved to generic decoder_t class?
        size_t bytes_read;

        //  Channel ID of the command being decoded.
        uint16_t channel;

        //  Message currently being decoded. message_offset points to how much
        //  data was already decoded into the message.
        message_t message;
        size_t message_offset;

        //  If true, messages may pass through the decoder. If false, only
        //  AMQP commands are passed (e.g. during initial AMQP handshaking).
        bool flow_on;

        //  Channel to receive message on.
        uint16_t message_channel;

        //  Buffer to read the frames in (excluding actual message content).
        unsigned char framebuf [i_amqp::frame_min_size];
        enum {framebuf_size = i_amqp::frame_min_size};

        amqp_decoder_t (const amqp_decoder_t&);
        void operator = (const amqp_decoder_t&);
    };

}

#endif

#endif
