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

#include <assert.h>
#include <algorithm>

#include "amqp09_encoder.hpp"
#include "i_amqp09.hpp"
#include "wire.hpp"

zmq::amqp09_encoder_t::amqp09_encoder_t (mux_t *mux_,
      amqp09_marshaller_t *marshaller_, bool server_,
      const char *out_exchange_, const char *out_routing_key_) :
    server (server_),
    mux (mux_),
    marshaller (marshaller_),
    flow_on (false),
    out_exchange (out_exchange_),
    out_routing_key (out_routing_key_)
{
    assert (out_exchange.size () <= 0xff);
    assert (out_routing_key.size () <= 0xff);

    tmpbuf_size = i_amqp09::frame_min_size;
    tmpbuf = (unsigned char*) malloc (tmpbuf_size);
    assert (tmpbuf);

    next_step (NULL, 0, &amqp09_encoder_t::message_ready);
}

zmq::amqp09_encoder_t::~amqp09_encoder_t ()
{
    free (tmpbuf);
}

void zmq::amqp09_encoder_t::flow (bool on_)
{
    flow_on = on_;
}

bool zmq::amqp09_encoder_t::message_ready ()
{
    //  Start encoding a command. Firstly, try to retrieve one command
    //  from the marshaller. If available, write its header.
    if (marshaller->read (&command))
    {
        size_t offset = 0;
        assert (offset + sizeof (uint8_t) <= tmpbuf_size);
        put_uint8 (tmpbuf + offset, i_amqp09::frame_method);
        offset += sizeof (uint8_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, 0);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint32_t) <= tmpbuf_size);
        put_uint32 (tmpbuf + offset, command.args_size + 4);
        offset += sizeof (uint32_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, command.class_id);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, command.method_id);
        offset += sizeof (uint16_t);
        next_step (tmpbuf, offset, &amqp09_encoder_t::command_header);
        return true;
    }

    //  If flow is off, don't even continue to the message encoding section.
    //  Return false to denote that there is nothing to encode.
    if (!flow_on)
        return false;

    //  Get one message from the dispatcher (via proxy).
    if (!mux->read (&message))
        return false;

    //  Encode method frame frame header
    size_t offset = 0;
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp09::frame_method);
    offset += sizeof (uint8_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint32_t) <= tmpbuf_size);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);

    //  Encode method frame payload (basic.deliver on AMQP broker, basic.publish
    //  on AMQP client).
    if (server) {
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, i_amqp09::basic_id);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, i_amqp09::basic_deliver_id);
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
        put_uint16 (tmpbuf + offset, i_amqp09::basic_id);
        offset += sizeof (uint16_t);
        assert (offset + sizeof (uint16_t) <= tmpbuf_size);
        put_uint16 (tmpbuf + offset, i_amqp09::basic_publish_id);
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

    //  Encode frame-end octet
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp09::frame_end);
    offset += sizeof (uint8_t);

    //  Fill in the size
    put_uint32 (tmpbuf + size_offset, offset - 8); 

    next_step (tmpbuf, offset, &amqp09_encoder_t::content_header);
    return true;
}

bool zmq::amqp09_encoder_t::command_header ()
{
    //  Encode command arguments. No processing is necessary as arguments are
    //  already converted into binary format by AMQP marshaller.
    next_step (command.args, command.args_size,
        &amqp09_encoder_t::command_arguments);
    return true;
}

bool zmq::amqp09_encoder_t::command_arguments ()
{
    //  Encode frame-end octet for the command. We can deallocate command
    //  arguments in command_t structure at this point as we won't need it
    //  any more.
    free (command.args);
    tmpbuf [0] = i_amqp09::frame_end;
    next_step (tmpbuf, 1, &amqp09_encoder_t::message_ready);
    return true;
}

bool zmq::amqp09_encoder_t::content_header ()
{
    //  Encode complete message header frame.
    size_t offset = 0;
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp09::frame_header);
    offset += sizeof (uint8_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint32_t) <= tmpbuf_size);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, i_amqp09::basic_id);
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
    put_uint8 (tmpbuf + offset, i_amqp09::frame_end);
    offset += sizeof (uint8_t);

    //  Fill in the size
    put_uint32 (tmpbuf + size_offset, offset - 8);
    
    body_offset = 0;
    next_step (tmpbuf, offset, &amqp09_encoder_t::content_body_frame_header);
    return true;
}

bool zmq::amqp09_encoder_t::content_body_frame_header ()
{
    //  Determine the size of data to transfer in the message body frame
    size_t body_size = std::min (message.size - body_offset,
        (size_t) (i_amqp09::frame_min_size - 8));
 
    //  Encode header of message body frame
    size_t offset = 0;
    assert (offset + sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf + offset, i_amqp09::frame_body);
    offset += sizeof (uint8_t);
    assert (offset + sizeof (uint16_t) <= tmpbuf_size);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    assert (offset + sizeof (uint32_t) <= tmpbuf_size);
    put_uint32 (tmpbuf + offset, body_size);
    offset += sizeof (uint32_t);

    next_step (tmpbuf, offset, &amqp09_encoder_t::content_body);
    return true;
}

bool zmq::amqp09_encoder_t::content_body ()
{
    //  Determine the size of data to transfer in the message body frame
    size_t body_size = std::min (message.size - body_offset,
        (size_t) (i_amqp09::frame_min_size - 8));

    //  Encode appropriate fragment of the message body fraction
    next_step ((unsigned char*) message.data + body_offset,
        body_size, &amqp09_encoder_t::frame_end);
    body_offset += body_size;
    return true;
}

bool zmq::amqp09_encoder_t::frame_end ()
{
    //  Encode frame-end octet for message body frame
    assert (sizeof (uint8_t) <= tmpbuf_size);
    put_uint8 (tmpbuf, i_amqp09::frame_end);

    //  If the message is transferred completely, start encoding new message,
    //  else start encoding the rest of the message into the next
    //  message body frame.
    if (message.size == body_offset)
        next_step (tmpbuf, sizeof (uint8_t),
            &amqp09_encoder_t::message_ready);
    else
        next_step (tmpbuf, sizeof (uint8_t),
            &amqp09_encoder_t::content_body_frame_header);
    return true;
}
