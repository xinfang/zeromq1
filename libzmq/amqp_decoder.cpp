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

#if defined ZMQ_HAVE_AMQP

#include <zmq/amqp_decoder.hpp>
#include <zmq/i_amqp.hpp>
#include <zmq/wire.hpp>

zmq::amqp_decoder_t::amqp_decoder_t (demux_t *demux_, i_amqp *callback_) :
    amqp_unmarshaller_t (callback_),
    demux (demux_)
{
    //  Wait for frame header to arrive.
    next_step (framebuf, 7, &amqp_decoder_t::method_frame_header_ready);
}

zmq::amqp_decoder_t::~amqp_decoder_t ()
{
}

bool zmq::amqp_decoder_t::method_frame_header_ready ()
{
    //  Method frame header arrived - parse it read method payload.
    uint8_t type = get_uint8 (framebuf);
    assert (type == i_amqp::frame_method);
    uint16_t channel = get_uint16 (framebuf + 1);
    bytes_read = get_uint32 (framebuf + 3);
    assert (bytes_read + 1 <= framebuf_size); 
    next_step (framebuf, bytes_read + 1,
        &amqp_decoder_t::method_payload_ready);
    return true;
}

bool zmq::amqp_decoder_t::method_payload_ready ()
{
    //  Method payload is read. Retrieve class and method id. 
    assert (bytes_read >= 5);
    uint16_t class_id = get_uint16 (framebuf);
    uint16_t method_id = get_uint16 (framebuf + 2);

    //  Check the frame-end octet.
    assert (framebuf [bytes_read] = i_amqp::frame_end);

    //  Determine whether frame is a command or beggining of a message
    //  If the former, forward it to the protocol state machine
    //  (via unmarshaller). If the latter, start reading message content header.
    if (class_id == i_amqp::basic_id && method_id == i_amqp::basic_deliver_id) {
       next_step (framebuf, 7,
           &amqp_decoder_t::content_header_frame_header_ready);
    }
    else {
        amqp_unmarshaller_t::write (class_id, method_id, framebuf + 4,
            bytes_read - 4);
        next_step (framebuf, 7, &amqp_decoder_t::method_frame_header_ready);
    }

    return true;
}

bool zmq::amqp_decoder_t::content_header_frame_header_ready ()
{
    //  Frame header of content header is read. Read the content header payload.
    uint8_t type = get_uint8 (framebuf);
    assert (type == i_amqp::frame_header);
    uint16_t channel = get_uint16 (framebuf + 1);
    assert (channel == 1);
    uint32_t bytes_read = get_uint32 (framebuf + 3);
    assert (bytes_read + 1 <= framebuf_size); 
    next_step (framebuf, bytes_read + 1,
        &amqp_decoder_t::content_header_payload_ready);
    return true;
}

bool zmq::amqp_decoder_t::content_header_payload_ready ()
{
    //  Content header frame read. Check it, allocate a buffer for the message
    //  and start reading message body.
    assert (bytes_read >= 14);
    uint16_t class_id = get_uint16 (framebuf);
    assert (class_id == i_amqp::basic_id);
    uint16_t weight = get_uint16 (framebuf + 2);
    assert (weight == 0);
    uint64_t body_size = get_uint64 (framebuf + 4);
    //  Ignore property flags at this point.

    //  Check the frame frame-end octet
    assert (framebuf [bytes_read] == i_amqp::frame_end);

    //  Allocate message large enough to hold the entire payload.
    message.rebuild (body_size);
    message_offset = 0;

    next_step (framebuf, 7, &amqp_decoder_t::content_body_frame_header_ready);
    return true;
}

bool zmq::amqp_decoder_t::content_body_frame_header_ready ()
{
    //  Frame header of message body frame is read. Start reading it's payload.
    //  Note that the data are read directly to the message buffer.
    uint8_t type = get_uint8 (framebuf);
    uint16_t channel = get_uint16 (framebuf + 1);
    assert (channel == 1);
    uint32_t size = get_uint32 (framebuf + 3);
    assert (type == i_amqp::frame_body);

    bytes_read = size;
    assert (message_offset + size <= message.size ());
    next_step (((unsigned char*) message.data ()) + message_offset, bytes_read,
        &amqp_decoder_t::content_body_payload_ready);
    return true;
}

bool zmq::amqp_decoder_t::content_body_payload_ready ()
{
    //  Payload of message body frame is read. Adjust offset and
    //  read the frame-end octet.
    message_offset += bytes_read;
    next_step (framebuf, 1, &amqp_decoder_t::content_body_frame_end_ready);
    return true;
}


bool zmq::amqp_decoder_t::content_body_frame_end_ready ()
{
    //  Message body frame read. If the message is complete. Wait for new
    //  command. Otherwise wait for next message body frame.
    assert (framebuf [0] == i_amqp::frame_end);

    if (message_offset == message.size ()) {
        if (!demux->write (message))
            return false;
        next_step (framebuf, 7, &amqp_decoder_t::method_frame_header_ready);
    }
    else
        next_step (framebuf, 7,
            &amqp_decoder_t::content_body_frame_header_ready);

    return true;
}

#endif
