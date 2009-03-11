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

#include <string.h>

#include <zmq/amqp_message.hpp>
#include <zmq/wire.hpp>
#include <zmq/i_amqp.hpp>

zmq::amqp_message_t::amqp_message_t (const char *exchange_,
    const char *routing_key_, int property_count_, const char **property_names_,
    const char **property_values_, size_t size_)
{
    //  Compute the size of message header.
    size_t exchange_size = strlen (exchange_);
    size_t routing_key_size = strlen (routing_key_);
    size_t header_size = sizeof (uint8_t) + exchange_size;
    header_size += sizeof (uint8_t) + routing_key_size;
    header_size += sizeof (uint64_t) + sizeof (uint32_t) + sizeof (uint16_t);
    if (property_count_) {
        header_size += sizeof (uint32_t);
        for (int i = 0; i != property_count_; i ++)
            header_size += sizeof (uint8_t) + strlen (property_names_ [i]) +
                sizeof (uint8_t) + sizeof (uint32_t) +
                strlen (property_values_ [i]);
    }

    //  Allocate the memory for the message.
    message_t::rebuild (header_size + size_);

    //  Fill in the message header.
    unsigned char *data = (unsigned char*) message_t::data ();
    assert (exchange_size <= 255);
    put_uint8 (data, exchange_size);
    data ++;
    memcpy (data, exchange_, exchange_size);
    assert (routing_key_size <= 255);
    data += exchange_size;
    
    put_uint8 (data, routing_key_size);
    data ++;
    memcpy (data, routing_key_, routing_key_size);
    data += routing_key_size;

    put_uint64 (data, size_);
    data += sizeof (uint64_t);

    unsigned char *props_size_pos = data;
    data += sizeof (uint32_t);

    if (!property_count_) {
        put_uint16 (data, 0);
        data += sizeof (uint16_t);
        put_uint32 (props_size_pos, sizeof (uint16_t));
    }
    else {
        put_uint16 (data, i_amqp::basic_property_headers);
        data += sizeof (uint16_t);
        unsigned char *size_pos = data;
        data += sizeof (uint32_t);
        size_t properties_size = 0;
        for (int i = 0; i != property_count_; i ++) {
            size_t name_size = strlen (property_names_ [i]);
            assert (name_size <= 255);
            size_t value_size = strlen (property_values_ [i]);
            put_uint8 (data, name_size);
            data += sizeof (uint8_t);
            memcpy (data, property_names_ [i], name_size);
            data += name_size;
            put_uint8 (data, 'S');
            data += sizeof (uint8_t);
            put_uint32 (data, value_size);
            data += sizeof (uint32_t);
            memcpy (data, property_values_ [i], value_size);
            data += value_size;

            properties_size += sizeof (uint8_t) + name_size + sizeof (uint8_t) +
                sizeof (uint32_t) + value_size;
        }
        put_uint32 (size_pos, properties_size);
        put_uint32 (props_size_pos, properties_size +
            sizeof (uint32_t) + sizeof (uint16_t));
    }
    body = data;
}

#endif
