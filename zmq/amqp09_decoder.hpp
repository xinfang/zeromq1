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

#ifndef __ZMQ_AMQP09_DECODER_HPP_INCLUDED__
#define __ZMQ_AMQP09_DECODER_HPP_INCLUDED__

#include "message.hpp"
#include "decoder.hpp"
#include "demux.hpp"
#include "amqp09_unmarshaller.hpp"

namespace zmq
{

    //  Decoder for AMQP version 0-9
    class amqp09_decoder_t : public decoder_t <amqp09_decoder_t>
    {
    public:

        //  Create the decoder. Specifies demux and unmarshaller to use.
        //  'sever' parameter specifies whether messages are expected to be
        //  carried by 'basic.publish' command (server) or 'basic.deliver'
        //  command (client)
        amqp09_decoder_t (demux_t *demux_, amqp09_unmarshaller_t *unmarshaller_,
            bool server_);
        ~amqp09_decoder_t ();

        //  Turns message flow on/off
        void flow (bool on_);

    private:

        void method_frame_header_ready ();
        void method_payload_ready ();
        void content_header_frame_header_ready ();
        void content_header_payload_ready ();
        void content_body_frame_header_ready ();
        void content_body_payload_ready ();
        void content_body_frame_end_ready ();

        demux_t *demux;
        amqp09_unmarshaller_t *unmarshaller;

        unsigned char tmpbuf [7];
        unsigned char *framebuf;
        size_t framebuf_size;
        size_t payload_size;
        size_t curr_body_size;
        size_t msg_data_off;
        message_t message;

        bool flow_on;
        bool server;
    };

}

#endif

