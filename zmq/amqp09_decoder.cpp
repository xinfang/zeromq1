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

#include "amqp09_decoder.hpp"
#include "i_amqp09.hpp"
#include "wire.hpp"

zmq::amqp09_decoder_t::amqp09_decoder_t (demux_t *demux_,
      amqp09_unmarshaller_t *unmarshaller_, bool server_) :
    demux (demux_),
    unmarshaller (unmarshaller_),
    flow_on (false),
    server (server_)
{
    //  Alocate buffer for the frames
    framebuf_size = i_amqp09::frame_min_size;
    framebuf = (unsigned char*) malloc (framebuf_size);
    assert (framebuf);

    //  Wait for frame header to arrive
    next_step (tmpbuf, 7, &amqp09_decoder_t::method_frame_header_ready);
}

zmq::amqp09_decoder_t::~amqp09_decoder_t ()
{
    free (framebuf);
}

void zmq::amqp09_decoder_t::flow (bool on_)
{
    flow_on = on_;
}

void zmq::amqp09_decoder_t::method_frame_header_ready ()
{
    //  Method frame header arrived - parse it read method payload
    uint8_t type = get_uint8 (tmpbuf);
    //  uint16_t channel = get_uint16 (tmpbuf + 1);
    uint32_t size = get_uint32 (tmpbuf + 3);
    assert (type == i_amqp09::frame_method);
    assert (size + 1 <= framebuf_size); 
    payload_size = size;
    next_step (framebuf, size + 1, &amqp09_decoder_t::method_payload_ready);
}

void zmq::amqp09_decoder_t::method_payload_ready ()
{
    //  Method payload is read. Retrieve class and method id. 
    assert (payload_size >= 4);
    uint16_t class_id = get_uint16 (framebuf);
    uint16_t method_id = get_uint16 (framebuf + 2);

    //  Check the frame-end octet
    assert (framebuf [payload_size] = i_amqp09::frame_end);

    //  Determine whether frame is a command or beggining of a message
    //  If the former, forward it to the protocol state machine
    //  (via unmarshaller). If the latter, start reading message content header.
    if ((server && class_id == i_amqp09::basic_id && method_id ==
         i_amqp09::basic_publish_id) || (!server && class_id ==
         i_amqp09::basic_id && method_id == i_amqp09::basic_deliver_id)) {
       assert (flow_on);
       next_step (tmpbuf, 7,
           &amqp09_decoder_t::content_header_frame_header_ready);
    }
    else {
        unmarshaller->write (class_id, method_id, framebuf + 4,
            payload_size - 4);
        next_step (tmpbuf, 7, &amqp09_decoder_t::method_frame_header_ready);
    }
}

void zmq::amqp09_decoder_t::content_header_frame_header_ready ()
{
    //  Frame header of content header is read. Read the content header payload.
    uint8_t type = get_uint8 (tmpbuf);
    //  uint16_t channel = get_uint16 (tmpbuf + 1);
    uint32_t size = get_uint32 (tmpbuf + 3);
    assert (type == i_amqp09::frame_header);
    assert (size + 1 <= framebuf_size); 
    payload_size = size;
    next_step (framebuf, size + 1,
        &amqp09_decoder_t::content_header_payload_ready);
}

void zmq::amqp09_decoder_t::content_header_payload_ready ()
{
    //  Content header frame read. Check it, allocate a buffer for the message
    //  and start reading message body.
    assert (payload_size >= 10);
    uint16_t class_id = get_uint16 (framebuf);
    uint64_t body_size = get_uint64 (framebuf + 4);
    assert (class_id == i_amqp09::basic_id);

    //  Check the frame frame-end octet
    assert (framebuf [payload_size] == i_amqp09::frame_end);

    message.rebuild (body_size);
    curr_body_size = 0;
    msg_data_off = 0;

    next_step (tmpbuf, 7, &amqp09_decoder_t::content_body_frame_header_ready);
}

void zmq::amqp09_decoder_t::content_body_frame_header_ready ()
{
    //  Frame header of message body frame is read. Start reading it's payload.
    //  Note that the data are read directly to the message buffer.
    uint8_t type = get_uint8 (tmpbuf);
    //  uint16_t channel = get_uint16 (tmpbuf + 1);
    uint32_t size = get_uint32 (tmpbuf + 3);
    assert (type == i_amqp09::frame_body);

    curr_body_size = size;
    assert (msg_data_off + size <= message.size ());
    next_step (((unsigned char*) message.data ()) + msg_data_off, size,
        &amqp09_decoder_t::content_body_payload_ready);
}

void zmq::amqp09_decoder_t::content_body_payload_ready ()
{
    //  Payload of message body frame is read. Read the frame-end octet.
    msg_data_off += curr_body_size;
    next_step (tmpbuf, 1, &amqp09_decoder_t::content_body_frame_end_ready);
}


void zmq::amqp09_decoder_t::content_body_frame_end_ready ()
{
    //  Message body frame read. If the message is complete. Wait for new
    //  command. Otherwise wait for next message body frame.
    assert (tmpbuf [0] == i_amqp09::frame_end);

    if (msg_data_off == message.size ()) {
        demux->write (message);
        next_step (tmpbuf, 7, &amqp09_decoder_t::method_frame_header_ready);
    }
    else
        next_step (tmpbuf, 7,
            &amqp09_decoder_t::content_body_frame_header_ready);
}
