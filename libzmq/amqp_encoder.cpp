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

#if defined ZMQ_HAVE_AMQP

#include <assert.h>
#include <algorithm>

#include <zmq/amqp_encoder.hpp>
#include <zmq/wire.hpp>

zmq::amqp_encoder_t::amqp_encoder_t (bool server_) :
    server (server_)
{
    next_step (NULL, 0, &amqp_encoder_t::message_ready, true);
}

zmq::amqp_encoder_t::~amqp_encoder_t ()
{
}

bool zmq::amqp_encoder_t::message_ready ()
{
    //  Start encoding a command. Firstly, try to retrieve one command
    //  from the marshaller. If available, write its header.
    amqp_marshaller_t::command_t command;
    if (marshaller.read (&command))
    {
        size_t offset = 0;

        //  Frame type is 'method'.
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, i_amqp::frame_method);
        offset += sizeof (uint8_t);

        //  Channel zero.
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, 0);
        offset += sizeof (uint16_t);

        //  Length of the frame (class + method + arguments).
        assert (offset + sizeof (uint32_t) <= tmpbuf_size);
        put_uint32 (tmpbuf + offset, command.args_size + sizeof (uint16_t) +
            sizeof (uint16_t));
        offset += sizeof (uint32_t);

        //  Class ID.
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, command.class_id);
        offset += sizeof (uint16_t);

        //  Method ID.
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, command.method_id);
        offset += sizeof (uint16_t);

        next_step (tmpbuf, offset, &amqp_encoder_t::command_header);
        return true;
    }

    //  There is no AMQP command available... Get a message to send.
    if (!proxy->read (source_engine_id, &message))
        return false;

    //  Encode method frame frame header.
    size_t offset = 0;

    //  Frame type: method.
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp::frame_method);
    offset += sizeof (uint8_t);

    //  Channel zero.
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);

    //  Leave frame length empty for now. To be filled in later when we know
    //  what the actual size is.
    assert (offset + sizeof (uint32_t) <= tmpbuf_size);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);

    //  Encode method frame payload (basic.deliver on AMQP broker, basic.publish
    //  on AMQP client).
//  TODO: Why are we doing this by hand? Maeshaller should do it automatically.
    if (server) {
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, i_amqp::basic_id);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, i_amqp::basic_deliver_id);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, 0);
        offset += sizeof (uint8_t);
        assert (offset + sizeof (uint64_t) <= tmpbuf_size);
        put_uint64 (tmpbuf + offset, 0);
        offset += sizeof (uint64_t);
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, 0);
        offset += sizeof (uint8_t);
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, (uint8_t) out_exchange.size ());
        offset += sizeof (uint8_t);
        assert (offset + out_exchange.size () <= tmpbuf_size);
        memcpy (tmpbuf + offset, out_exchange.c_str (), out_exchange.size ());
        offset += out_exchange.size ();
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, (uint8_t) out_routing_key.size ());
        offset += sizeof (uint8_t);
        assert (offset + out_routing_key.size () <= tmpbuf_size);
        memcpy (tmpbuf + offset, out_routing_key.c_str (),
            out_routing_key.size ());
        offset += out_routing_key.size ();
    }
    else {
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, i_amqp::basic_id);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, i_amqp::basic_publish_id);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, 0);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, (uint8_t) out_exchange.size ());
        offset += sizeof (uint8_t);
        assert (offset + out_exchange.size () <= tmpbuf_size);
        memcpy (tmpbuf + offset, out_exchange.c_str (), out_exchange.size ());
        offset += out_exchange.size ();
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, (uint8_t) out_routing_key.size ());
        offset += sizeof (uint8_t);
        assert (offset + out_routing_key.size () <= tmpbuf_size);
        memcpy (tmpbuf + offset, out_routing_key.c_str (),
            out_routing_key.size ());
        offset += out_routing_key.size ();
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, 0);
        offset += sizeof (uint8_t);
    }

    //  Encode frame-end octet.
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp::frame_end);
    offset += sizeof (uint8_t);

    //  Now we know what the size of the frame is. Fill it in.
    put_uint32 (tmpbuf + size_offset, offset - 8); 

    next_step (tmpbuf, offset, &amqp_encoder_t::content_header);
    return true;
}

bool zmq::amqp_encoder_t::command_header ()
{
    //  Encode command arguments. No processing is necessary as arguments are
    //  already converted into binary format by AMQP marshaller.
    next_step (command.args, command.args_size,
        &amqp_encoder_t::command_arguments);
    return true;
}

bool zmq::amqp_encoder_t::command_arguments ()
{
    //  Encode frame-end octet for the command. We can deallocate command
    //  arguments in command_t structure at this point as we won't need it
    //  any more.
    free (command.args);
    tmpbuf [0] = i_amqp::frame_end;
    next_step (tmpbuf, 1, &amqp_encoder_t::message_ready);
    return true;
}

bool zmq::amqp_encoder_t::content_header ()
{
    //  Encode minimal message header frame.
    //  No special message properties are used.
    size_t offset = 0;
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp::frame_header);
    offset += sizeof (uint8_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint32_t) <= tmpbuf_size);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, i_amqp::basic_id);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint64_t) <= tmpbuf_size);
    put_uint64 (tmpbuf + offset, message.size);
    offset += sizeof (uint64_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp::frame_end);
    offset += sizeof (uint8_t);

    //  Fill in the size
    put_uint32 (tmpbuf + size_offset, offset - 8);
    
    body_offset = 0;
    next_step (tmpbuf, offset, &amqp_encoder_t::content_body_frame_header);
    return true;
}

bool zmq::amqp_encoder_t::content_body_frame_header ()
{
    //  Determine the size of data to transfer in the message body frame
    size_t body_size = std::min (message.size - body_offset,
        (size_t) (i_amqp::frame_min_size - 8));
 
    //  Encode header of message body frame
    size_t offset = 0;
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp::frame_body);
    offset += sizeof (uint8_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint32_t) <= tmpbuf_size);
    put_uint32 (tmpbuf + offset, body_size);
    offset += sizeof (uint32_t);

    next_step (tmpbuf, offset, &amqp_encoder_t::content_body);
    return true;
}

bool zmq::amqp_encoder_t::content_body ()
{
    //  Determine the size of data to transfer in the message body frame
    size_t body_size = std::min (message.size - body_offset,
        (size_t) (i_amqp::frame_min_size - 8));

    //  Encode appropriate fragment of the message body fraction
    next_step ((unsigned char*) message.data + body_offset,
        body_size, &amqp_encoder_t::frame_end);
    body_offset += body_size;
    return true;
}

bool zmq::amqp_encoder_t::frame_end ()
{
    //  Encode frame-end octet for message body frame
    assert (sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf, i_amqp::frame_end);

    //  If the message is transferred completely, start encoding new message,
    //  else start encoding the rest of the message into the next
    //  message body frame.
    if (message.size == body_offset)
        next_step (tmpbuf, sizeof (uint8_t),
            &amqp_encoder_t::message_ready);
    else
        next_step (tmpbuf, sizeof (uint8_t),
            &amqp_encoder_t::content_body_frame_header);
    return true;
}

#endif
