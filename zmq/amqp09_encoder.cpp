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
#include "amqp09_constants.hpp"
#include "wire.hpp"

zmq::amqp09_encoder_t::amqp09_encoder_t (dispatcher_proxy_t *proxy_,
    int source_thread_id_, amqp09_marshaller_t *marshaller_, bool server_,
    const char *out_exchange_, const char *out_routing_key_) :
    server (server_),
    proxy (proxy_),
    source_thread_id (source_thread_id_),
    marshaller (marshaller_),
    flow_on (false),
    out_exchange (out_exchange_),
    out_routing_key (out_routing_key_)
{
    assert (out_exchange.size () <= 0xff);
    assert (out_routing_key.size () <= 0xff);

    tmpbuf_size = amqp09::frame_min_size;
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
    if (marshaller->read (&command))
    {
        size_t offset = 0;
        put_uint8 (tmpbuf + offset, amqp09::frame_method);
        offset += sizeof (uint8_t);
        put_uint16 (tmpbuf + offset, 0);
        offset += sizeof (uint16_t);
        put_uint32 (tmpbuf + offset, command.args_size + 4);
        offset += sizeof (uint32_t);
        put_uint16 (tmpbuf + offset, command.class_id);
        offset += sizeof (uint16_t);
        put_uint16 (tmpbuf + offset, command.method_id);
        offset += sizeof (uint16_t);
        next_step (tmpbuf, offset, &amqp09_encoder_t::command_header);
        return true;
    }

    if (!flow_on)
        return false;

    if (!proxy->read (source_thread_id, &message))
        return false;

    size_t offset = 0;
    put_uint8 (tmpbuf + offset, amqp09::frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);

    if (server) {
        put_uint16 (tmpbuf + offset, amqp09::basic);
        offset += sizeof (uint16_t);
        put_uint16 (tmpbuf + offset, amqp09::basic_deliver);
        offset += sizeof (uint16_t);
        put_uint8 (tmpbuf + offset, 0);
        offset += sizeof (uint8_t);
        put_uint64 (tmpbuf + offset, 0);
        offset += sizeof (uint64_t);
        put_uint8 (tmpbuf + offset, 0);
        offset += sizeof (uint8_t);
        put_uint8 (tmpbuf + offset, (uint8_t) out_exchange.size ());
        offset += sizeof (uint8_t);
        memcpy (tmpbuf + offset, out_exchange.c_str (), out_exchange.size ());
        offset += out_exchange.size ();
        put_uint8 (tmpbuf + offset, (uint8_t) out_routing_key.size ());
        offset += sizeof (uint8_t);
        memcpy (tmpbuf + offset, out_routing_key.c_str (),
            out_routing_key.size ());
        offset += out_routing_key.size ();
    }
    else {
        put_uint16 (tmpbuf + offset, amqp09::basic);
        offset += sizeof (uint16_t);
        put_uint16 (tmpbuf + offset, amqp09::basic_publish);
        offset += sizeof (uint16_t);
        put_uint16 (tmpbuf + offset, 0);
        offset += sizeof (uint16_t);
        put_uint8 (tmpbuf + offset, (uint8_t) out_exchange.size ());
        offset += sizeof (uint8_t);
        memcpy (tmpbuf + offset, out_exchange.c_str (), out_exchange.size ());
        offset += out_exchange.size ();
        put_uint8 (tmpbuf + offset, (uint8_t) out_routing_key.size ());
        offset += sizeof (uint8_t);
        memcpy (tmpbuf + offset, out_routing_key.c_str (),
            out_routing_key.size ());
        offset += out_routing_key.size ();
        tmpbuf [offset] = 0;
        offset += sizeof (uint8_t);
    }
    put_uint8 (tmpbuf + offset, amqp09::frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (tmpbuf + size_offset, offset - 8); 

    next_step (tmpbuf, offset, &amqp09_encoder_t::content_header);
    return true;
}

bool zmq::amqp09_encoder_t::command_header ()
{
    next_step (command.args, command.args_size,
        &amqp09_encoder_t::command_arguments);
    return true;
}

bool zmq::amqp09_encoder_t::command_arguments ()
{
    free (command.args);
    tmpbuf [0] = amqp09::frame_end;
    next_step (tmpbuf, 1, &amqp09_encoder_t::message_ready);
    return true;
}

bool zmq::amqp09_encoder_t::content_header ()
{
    size_t offset = 0;
    put_uint8 (tmpbuf + offset, amqp09::frame_header);
    offset += sizeof (uint8_t);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    size_t size_offset = offset;
    offset += sizeof (uint32_t);
    put_uint16 (tmpbuf + offset, amqp09::basic);
    offset += sizeof (uint16_t);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    put_uint64 (tmpbuf + offset, message.size);
    offset += sizeof (uint64_t);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    put_uint8 (tmpbuf + offset, amqp09::frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (tmpbuf + size_offset, offset - 8);
    
    body_offset = 0;
    next_step (tmpbuf, offset, &amqp09_encoder_t::content_body_frame_header);
    return true;
}

bool zmq::amqp09_encoder_t::content_body_frame_header ()
{
    size_t body_size = std::min (message.size - body_offset,
        (size_t) (amqp09::frame_min_size - 8));
 
    size_t offset = 0;
    put_uint8 (tmpbuf + offset, amqp09::frame_body);
    offset += sizeof (uint8_t);
    put_uint16 (tmpbuf + offset, 0);
    offset += sizeof (uint16_t);
    put_uint32 (tmpbuf + offset, body_size);
    offset += sizeof (uint32_t);

    next_step (tmpbuf, offset, &amqp09_encoder_t::content_body);
    return true;
}

bool zmq::amqp09_encoder_t::content_body ()
{
    size_t body_size = std::min (message.size - body_offset,
        (size_t) (amqp09::frame_min_size - 8));

    next_step ((unsigned char*) message.data + body_offset,
        body_size, &amqp09_encoder_t::frame_end);
    body_offset += body_size;
    return true;
}

bool zmq::amqp09_encoder_t::frame_end ()
{
    put_uint8 (tmpbuf, amqp09::frame_end);

    if (message.size == body_offset)
        next_step (tmpbuf, sizeof (uint8_t),
            &amqp09_encoder_t::message_ready);
    else
        next_step (tmpbuf, sizeof (uint8_t),
            &amqp09_encoder_t::content_body_frame_header);
    return true;
}
