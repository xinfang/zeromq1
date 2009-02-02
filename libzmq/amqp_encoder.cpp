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

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <assert.h>
#include <algorithm>

#include <zmq/amqp_encoder.hpp>
#include <zmq/wire.hpp>

zmq::amqp_encoder_t::amqp_encoder_t (mux_t *mux_, const char *queue_) :
    mux (mux_),
    queue (queue_),
    flow_on (false),
    message_channel (0)
{
    command.args = NULL;

    //  Encode the protocol header (AMQP/0-9-1) and stat the normal workflow.
    const char *protocol_header = "AMQP\x00\x00\x09\x01";
    next_step ((void*) protocol_header, 8,
        &amqp_encoder_t::message_ready, true);
}

zmq::amqp_encoder_t::~amqp_encoder_t ()
{
    if (command.args)
        free (command.args);
}

void zmq::amqp_encoder_t::flow (bool flow_on_, uint16_t channel_)
{
    flow_on = flow_on_;
    message_channel = channel_;
}

bool zmq::amqp_encoder_t::message_ready ()
{
    //  Start encoding a command. Firstly, try to retrieve one command
    //  from the marshaller. If available, write its header.
    if (amqp_marshaller_t::read (&command))
    {
        size_t offset = 0;

        //  Frame type is 'method'.
        assert (offset + sizeof (uint8_t) <= framebuf_size);
        put_uint8 (framebuf + offset, i_amqp::frame_method);
        offset += sizeof (uint8_t);

        //  Channel ID.
        assert (offset + sizeof (uint16_t) <= framebuf_size);
        put_uint16 (framebuf + offset, command.channel);
        offset += sizeof (uint16_t);

        //  Length of the frame (class + method + arguments).
        assert (offset + sizeof (uint32_t) <= framebuf_size);
        put_uint32 (framebuf + offset, command.args_size + sizeof (uint16_t) +
            sizeof (uint16_t));
        offset += sizeof (uint32_t);

        //  Class ID.
        assert (offset + sizeof (uint16_t) <= framebuf_size);
        put_uint16 (framebuf + offset, command.class_id);
        offset += sizeof (uint16_t);

        //  Method ID.
        assert (offset + sizeof (uint16_t) <= framebuf_size);
        put_uint16 (framebuf + offset, command.method_id);
        offset += sizeof (uint16_t);

        next_step (framebuf, offset, &amqp_encoder_t::command_header, true);
        return true;
    }

    //  There is no AMQP command available... Get a message to send.
    if (!flow_on || !mux->read (&message))
        return false;

    //  Encode method frame frame header.
    size_t offset = 0;

    //  Frame type: method.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, i_amqp::frame_method);
    offset += sizeof (uint8_t);

    //  Channel ID.
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, message_channel);
    offset += sizeof (uint16_t);

    //  Leave frame length empty for now. To be filled in later when we know
    //  what the actual size is.
    assert (offset + sizeof (uint32_t) <= framebuf_size);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);

    //  Encode method frame payload. In theory, this could be done via
    //  marshaller, however, doing it this way we can avoid a memory allocation
    //  and copying the payload.

    //  Basic.Publish
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, i_amqp::basic_id);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, i_amqp::basic_publish_id);
    offset += sizeof (uint16_t);

    //  Ticket (deprecated).
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, 0);
    offset += sizeof (uint16_t);

    //  Default exchange.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, 0);
    offset += sizeof (uint8_t);

    //  Routing key.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, (uint8_t) queue.size ());
    offset += sizeof (uint8_t);
    assert (offset + queue.size () <= framebuf_size);
    memcpy (framebuf + offset, queue.c_str (), queue.size ());
    offset += queue.size ();

    //  Mandatory = false, immediate = false.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, 0);
    offset += sizeof (uint8_t);

    //  Encode frame-end octet.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, i_amqp::frame_end);
    offset += sizeof (uint8_t);

    //  Now we know what the size of the frame is. Fill it in.
    put_uint32 (framebuf + size_offset, offset - 8); 

    next_step (framebuf, offset, &amqp_encoder_t::content_header, false);
    return true;
}

bool zmq::amqp_encoder_t::command_header ()
{
    //  Encode command arguments. No processing is necessary as arguments are
    //  already converted into binary format by AMQP marshaller.
    next_step (command.args, command.args_size,
        &amqp_encoder_t::command_arguments, false);
    return true;
}

bool zmq::amqp_encoder_t::command_arguments ()
{
    //  Encode frame-end octet for the command. We can deallocate command
    //  arguments in command_t structure at this point as we won't need it
    //  any more.
    free (command.args);
    command.args = NULL;
    framebuf [0] = i_amqp::frame_end;
    next_step (framebuf, 1, &amqp_encoder_t::message_ready, false);
    return true;
}

bool zmq::amqp_encoder_t::content_header ()
{
    //  Encode minimal message header frame.
    //  No special message properties are used.
    size_t offset = 0;

    //  Frame type: content header.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, i_amqp::frame_header);
    offset += sizeof (uint8_t);

    //  Channel ID.
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, message_channel);
    offset += sizeof (uint16_t);

    //  Leave frame size empty. To be filled in later.
    assert (offset + sizeof (uint32_t) <= framebuf_size);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);

    //  Content class: Basic.
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, i_amqp::basic_id);
    offset += sizeof (uint16_t);

    //  Weight. Unused.
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, 0);
    offset += sizeof (uint16_t);

    //  Message size.
    assert (offset + sizeof (uint64_t) <= framebuf_size);
    put_uint64 (framebuf + offset, message.size ());
    offset += sizeof (uint64_t);

    //  No properties.
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, 0);
    offset += sizeof (uint16_t);

    //  Frame-end octet.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, i_amqp::frame_end);
    offset += sizeof (uint8_t);

    //  Fill in the frame size.
    put_uint32 (framebuf + size_offset, offset - 8);
    
    message_offset = 0;
    next_step (framebuf, offset, &amqp_encoder_t::content_body_frame_header,
        false);
    return true;
}

bool zmq::amqp_encoder_t::content_body_frame_header ()
{
    //  Determine the size of data to transfer in the message body frame
    size_t body_size = std::min (message.size () - message_offset,
        (size_t) (i_amqp::frame_min_size - 8));
 
    //  Encode header of message body frame
    size_t offset = 0;

    //  Frame type: Message body.
    assert (offset + sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf + offset, i_amqp::frame_body);
    offset += sizeof (uint8_t);

    //  Channel ID.
    assert (offset + sizeof (uint16_t) <= framebuf_size);
    put_uint16 (framebuf + offset, message_channel);
    offset += sizeof (uint16_t);

    //  Frame size.
    assert (offset + sizeof (uint32_t) <= framebuf_size);
    put_uint32 (framebuf + offset, body_size);
    offset += sizeof (uint32_t);

    next_step (framebuf, offset, &amqp_encoder_t::content_body, false);
    return true;
}

bool zmq::amqp_encoder_t::content_body ()
{
    //  Determine the size of data to transfer in the message body frame.
    size_t body_size = std::min (message.size () - message_offset,
        (size_t) (i_amqp::frame_min_size - 8));

    //  Encode appropriate fragment of the message body fraction.
    next_step ((unsigned char*) message.data () + message_offset,
        body_size, &amqp_encoder_t::frame_end, false);
    message_offset += body_size;
    return true;
}

bool zmq::amqp_encoder_t::frame_end ()
{
    //  Encode frame-end octet for message body frame.
    assert (sizeof (uint8_t) <= framebuf_size);
    put_uint8 (framebuf, i_amqp::frame_end);

    //  If the message is transferred completely, start encoding new
    //  command/message. Otherwise start encoding the rest of the message into
    //  the next message body frame.
    if (message.size () == message_offset)
        next_step (framebuf, sizeof (uint8_t),
            &amqp_encoder_t::message_ready, true);
    else
        next_step (framebuf, sizeof (uint8_t),
            &amqp_encoder_t::content_body_frame_header, false);
    return true;
}

#endif
