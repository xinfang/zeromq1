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

#ifndef __ZMQ_BP_DECODER_HPP_INCLUDED__
#define __ZMQ_BP_DECODER_HPP_INCLUDED__

#include "dispatcher.hpp"
#include "dispatcher_proxy.hpp"
#include "decoder.hpp"

namespace zmq
{

    class amqp_decoder_t : public decoder_t <amqp_decoder_t>
    {
    protected:

        //  Frame header size
        enum {AMQP_frame_header_size = 7};
        
        //  End of the frame
        enum {AMQP_frame_end = 0xCE};
        
        //  AMQP frame types
        enum {
            AMQP_method_frame = 1,
            AMQP_content_header_frame = 2,
            AMQP_content_body_frame = 3
        };
               
        //  AMQP classes IDs
        enum {
            AMQP_basic = 60
        };

        //  AMQP methods IDs
        enum {
            AMQP_basic_publish = 40
        };
        //  Internal structure to hold frame header
        struct frame_header_t
        {
            uint8_t type;
            uint16_t channel;
            uint32_t size;
        };

        //  Internal structure to hold method payload class id & method id
        struct method_payload_t
        {
            uint16_t class_id;
            uint16_t method_id;
        };

        //  Internal structure to hold basic content header frame
        struct basic_content_header_payload_t
        {
            uint16_t class_id; 
            uint16_t weight;
            uint64_t body_size;
//            uint16_t prop_flags;
//            basic_content_prop_t prop;
        };
  
    public:

        amqp_decoder_t (size_t max_frame_size_, 
              dispatcher_proxy_t *dispatcher_proxy_, int destination_thread_id_);

        ~amqp_decoder_t ()
        {
            free (frame_buff);
        }
    protected:
        void parse_frame_header (frame_header_t *frame_header_);
        void method_frame_header_ready ();
        void content_header_frame_header_ready ();
        void content_body_frame_header_ready ();

        void method_payload_ready ();
        void content_header_payload_ready ();
        void content_body_payload_ready ();
        void message_ready ();

        unsigned char header_buff [AMQP_frame_header_size];
        unsigned char *frame_buff;

        // size of the content body part which is going to be read
        size_t  curr_body_size;

        // offset in msg.data
        size_t msg_data_off;

        cmsg_t msg;
    };

}

#endif
