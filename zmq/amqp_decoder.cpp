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

#include "amqp_decoder.hpp"
#include "wire.hpp"

#ifdef ZMQ_AMQP_DEBUG
    #include <string>
#endif

zmq::amqp_decoder_t::amqp_decoder_t (size_t max_frame_size_, 
      dispatcher_proxy_t *dispatcher_proxy_, int destination_thread_id_) :
    decoder_t <amqp_decoder_t> (dispatcher_proxy_, destination_thread_id_), 
    curr_body_size (0), msg_data_off (0)
{
    frame_buff = (unsigned char*)malloc (max_frame_size_);
    assert (frame_buff);

    init_cmsg (msg);

    next_step (header_buff, AMQP_frame_header_size, &amqp_decoder_t::method_frame_header_ready);
}

void zmq::amqp_decoder_t::parse_frame_header (frame_header_t *frame_header_)
{
    size_t  pos = 0;

    frame_header_->type = get_uint8 (header_buff);
    pos += sizeof (uint8_t);

    frame_header_->channel = get_uint16 (header_buff + pos);
    pos += sizeof (uint16_t);

    frame_header_->size = get_uint32 (header_buff + pos);
    pos += sizeof (uint32_t);
}


void zmq::amqp_decoder_t::method_frame_header_ready ()
{
    frame_header_t frame_header;

    parse_frame_header (&frame_header);

    assert (frame_header.type == AMQP_method_frame);

    #ifdef ZMQ_AMQP_DEBUG
    printf ("method frame header, size to read %i, %s(%i)\n", frame_header.size, __FILE__, __LINE__);
    #endif

    next_step (frame_buff, frame_header.size + 1, &amqp_decoder_t::method_payload_ready);
}

void zmq::amqp_decoder_t::content_header_frame_header_ready ()
{
    frame_header_t frame_header;

    parse_frame_header (&frame_header);

    assert (frame_header.type == AMQP_content_header_frame);

    #ifdef ZMQ_AMQP_DEBUG
    printf ("content header frame header, size to read %i, %s(%i)\n", frame_header.size, __FILE__, __LINE__);
    #endif

    next_step (frame_buff, frame_header.size + 1, &amqp_decoder_t::content_header_payload_ready);
}

void zmq::amqp_decoder_t::content_body_frame_header_ready ()
{
    frame_header_t frame_header;

    parse_frame_header (&frame_header);

    assert (frame_header.type == AMQP_content_body_frame);

    #ifdef ZMQ_AMQP_DEBUG
    printf ("content body frame header, size to read %i, %s(%i)\n", frame_header.size, __FILE__, __LINE__);
    #endif

    curr_body_size = frame_header.size; 

    next_step (frame_buff, curr_body_size + 1, &amqp_decoder_t::content_body_payload_ready);
}


void zmq::amqp_decoder_t::method_payload_ready ()
{
    
    method_payload_t method_payload;
    size_t pos = 0;

    method_payload.class_id = get_uint16 (frame_buff);
    pos += sizeof (uint16_t);

    method_payload.method_id = get_uint16 (frame_buff + pos);
    pos += sizeof (uint16_t);

    switch (method_payload.class_id) {
        case AMQP_basic:
            switch (method_payload.method_id) {
                case AMQP_basic_publish:
                    //60-40 AMQP_basic_publish
                    #ifdef ZMQ_AMQP_DEBUG
                    printf ("60-40 AMQP_basic_publish, %s(%i)\n", __FILE__, __LINE__);
                    #endif
                    
                    next_step (header_buff, AMQP_frame_header_size, &amqp_decoder_t::content_header_frame_header_ready);

                    break;
                default:
                    assert (0);
            } // end (method_payload.method_id)
            break;
        default: 
            assert (0);
    } // end switch (method_payload.class_id)
}

void zmq::amqp_decoder_t::content_header_payload_ready ()
{
    basic_content_header_payload_t content_header_payload;
    size_t pos = 0;

    content_header_payload.class_id = get_uint16 (frame_buff);
    pos += sizeof (uint16_t);

    content_header_payload.weight = get_uint16 (frame_buff + pos);
    pos += sizeof (uint16_t);

    content_header_payload.body_size = get_uint64 (frame_buff + pos);
    pos += sizeof (uint64_t);

    switch (content_header_payload.class_id) {
        case AMQP_basic:
            #ifdef ZMQ_AMQP_DEBUG
            printf ("basic content header, entire body_size %i, %s(%i)\n", content_header_payload.body_size, __FILE__, __LINE__);
            #endif

            msg.size = content_header_payload.body_size;
            msg.data = malloc (content_header_payload.body_size);
            assert (msg.data);
            msg.ffn = free;
           
            curr_body_size = 0;
            msg_data_off = 0;

            next_step (header_buff, AMQP_frame_header_size, &amqp_decoder_t::content_body_frame_header_ready);

            break;
        default:
            assert (0);
    }
}

void zmq::amqp_decoder_t::content_body_payload_ready ()
{
    #ifdef ZMQ_AMQP_DEBUG
    printf ("content body, entire body_size %i, curr body size %i, %s(%i)\n", 
          msg.size, curr_body_size, __FILE__, __LINE__);
    #endif

    memcpy ((unsigned char*)msg.data + msg_data_off, frame_buff, curr_body_size);

    msg_data_off += curr_body_size;

    if (msg_data_off == msg.size) {
        #ifdef ZMQ_AMQP_DEBUG
        printf ("have entire message %i/%i, %s(%i)\n", msg_data_off, msg.size, __FILE__, __LINE__);
        #endif

        message_ready (); 

    } else if (msg_data_off < msg.size) {
        #ifdef ZMQ_AMQP_DEBUG
        printf ("do not have entire message %i/%i, %s(%i)\n", msg_data_off, msg.size, __FILE__, __LINE__);
        #endif
        next_step (header_buff, AMQP_frame_header_size, &amqp_decoder_t::content_body_frame_header_ready);
    } else {
        assert (0);
    }
}

void zmq::amqp_decoder_t::message_ready ()
{
    #ifdef ZMQ_AMQP_DEBUG
    std::string msg_text ((char*)msg.data, msg.size);
    printf ("message data: \'%s\', %s(%i)\n", msg_text.c_str (), __FILE__, __LINE__);
    #endif

    done (msg);
    next_step (header_buff, AMQP_frame_header_size, &amqp_decoder_t::method_frame_header_ready);
}

