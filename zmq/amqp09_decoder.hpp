/*
    Copyright (c) 2007 FastMQ Inc.

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

#include "dispatcher.hpp"
#include "dispatcher_proxy.hpp"
#include "decoder.hpp"

namespace zmq
{

    class amqp09_decoder_t : public decoder_t <amqp09_decoder_t>
    {  
    public:

        amqp09_decoder_t (size_t max_frame_size_, 
              dispatcher_proxy_t *dispatcher_proxy_,
              int destination_thread_id_);
        ~amqp09_decoder_t ();

    protected:

        enum {
            amqp_method_frame = 1,
            amqp_content_header_frame = 2,
            amqp_content_body_frame = 3
        };
        enum {amqp_basic = 60};
        enum {amqp_basic_publish = 40};
        enum {amqp_frame_end = 0xce};

        void method_frame_header_ready ();
        void method_payload_ready ();
        void content_header_frame_header_ready ();
        void content_header_payload_ready ();
        void content_body_frame_header_ready ();
        void content_body_payload_ready ();
        void content_body_frame_end_ready ();

        unsigned char tmpbuf [7];
        unsigned char *framebuf;
        size_t framebuf_size;
        size_t  curr_body_size;
        size_t msg_data_off;
        cmsg_t msg;
    };

}

#endif
