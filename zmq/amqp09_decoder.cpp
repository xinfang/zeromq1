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

#include "amqp09_decoder.hpp"
#include "wire.hpp"

zmq::amqp09_decoder_t::amqp09_decoder_t (size_t max_frame_size_, 
      dispatcher_proxy_t *dispatcher_proxy_, int destination_thread_id_) :
    decoder_t <amqp09_decoder_t> (dispatcher_proxy_, destination_thread_id_), 
    curr_body_size (0), msg_data_off (0), framebuf_size (max_frame_size_)
{
    framebuf = (unsigned char*) malloc (framebuf_size);
    assert (framebuf);
    init_cmsg (msg);
    next_step (tmpbuf, 7, &amqp09_decoder_t::method_frame_header_ready);
}

zmq::amqp09_decoder_t::~amqp09_decoder_t ()
{
    free_cmsg (msg);
    free (framebuf);
}

void zmq::amqp09_decoder_t::method_frame_header_ready ()
{
    uint8_t type = get_uint8 (tmpbuf);
    uint16_t channel = get_uint16 (tmpbuf + 1);
    uint32_t size = get_uint32 (tmpbuf + 3);
    assert (type == amqp_method_frame);
    assert (size + 1 <= framebuf_size); 
    next_step (framebuf, size + 1, &amqp09_decoder_t::method_payload_ready);
}

void zmq::amqp09_decoder_t::method_payload_ready ()
{  
    uint16_t class_id = get_uint16 (framebuf);
    uint16_t method_id = get_uint16 (framebuf + 2);
    assert (class_id == amqp_basic && method_id == amqp_basic_publish);
    next_step (tmpbuf, 7, &amqp09_decoder_t::content_header_frame_header_ready);
}

void zmq::amqp09_decoder_t::content_header_frame_header_ready ()
{
    uint8_t type = get_uint8 (tmpbuf);
    uint16_t channel = get_uint16 (tmpbuf + 1);
    uint32_t size = get_uint32 (tmpbuf + 3);
    assert (type == amqp_content_header_frame);
    assert (size + 1 <= framebuf_size); 
    next_step (framebuf, size + 1,
        &amqp09_decoder_t::content_header_payload_ready);
}

void zmq::amqp09_decoder_t::content_header_payload_ready ()
{
    uint16_t class_id = get_uint16 (framebuf);
    uint64_t body_size = get_uint64 (framebuf + 4);
    assert (class_id == amqp_basic);

    msg.size = body_size;
    msg.data = malloc (body_size);
    assert (msg.data);
    msg.ffn = free;           
    curr_body_size = 0;
    msg_data_off = 0;

    next_step (tmpbuf, 7, &amqp09_decoder_t::content_body_frame_header_ready);
}

void zmq::amqp09_decoder_t::content_body_frame_header_ready ()
{
    uint8_t type = get_uint8 (tmpbuf);
    uint16_t channel = get_uint16 (tmpbuf + 1);
    uint32_t size = get_uint32 (tmpbuf + 3);
    assert (type == amqp_content_body_frame);

    curr_body_size = size;
    assert (msg_data_off + size <= msg.size);
    next_step (((unsigned char*) msg.data) + msg_data_off, size,
        &amqp09_decoder_t::content_body_payload_ready);
}

void zmq::amqp09_decoder_t::content_body_payload_ready ()
{
    msg_data_off += curr_body_size;
    next_step (tmpbuf, 1, &amqp09_decoder_t::content_body_frame_end_ready);
}


void zmq::amqp09_decoder_t::content_body_frame_end_ready ()
{
    assert (tmpbuf [0] == amqp_frame_end);

    if (msg_data_off == msg.size) {
        done (msg);
        next_step (tmpbuf, 7, &amqp09_decoder_t::method_frame_header_ready);
    }
    else
        next_step (tmpbuf, 7,
            &amqp09_decoder_t::content_body_frame_header_ready);
}


