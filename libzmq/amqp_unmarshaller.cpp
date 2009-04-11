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

    This file is generated from amqp0-9-1.xml by amqp.gsl using GSL/4.
*/

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <zmq/amqp_unmarshaller.hpp>
#include <zmq/formatting.hpp>
#include <zmq/wire.hpp>

void zmq::amqp_unmarshaller_t::write (uint16_t channel_, uint16_t class_id,
            uint16_t method_id, unsigned char *args, size_t args_size)
{
    size_t offset = 0;

    switch (class_id) {
    case i_amqp::connection_id:
        switch (method_id) {
        case i_amqp::connection_start_id:
            {
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                uint8_t version_major = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                uint8_t version_minor = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                i_amqp::field_table_t server_properties;
                get_field_table (args, args_size, offset, server_properties);
                i_amqp::longstr_t mechanisms;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                mechanisms.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + mechanisms.size <= args_size);
                mechanisms.data = (void*) (args + offset);
                offset += mechanisms.size;
                i_amqp::longstr_t locales;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                locales.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + locales.size <= args_size);
                locales.data = (void*) (args + offset);
                offset += locales.size;

                callback->connection_start (  channel_
,
                    version_major,
                    version_minor,
                    server_properties,
                    mechanisms,
                    locales);

                return;
            }
        case i_amqp::connection_start_ok_id:
            {
                i_amqp::field_table_t client_properties;
                get_field_table (args, args_size, offset, client_properties);
                i_amqp::shortstr_t mechanism;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                mechanism.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + mechanism.size <= args_size);
                mechanism.data = (char*) (args + offset);
                offset += mechanism.size;
                i_amqp::longstr_t response;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                response.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + response.size <= args_size);
                response.data = (void*) (args + offset);
                offset += response.size;
                i_amqp::shortstr_t locale;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                locale.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + locale.size <= args_size);
                locale.data = (char*) (args + offset);
                offset += locale.size;

                callback->connection_start_ok (  channel_
,
                    client_properties,
                    mechanism,
                    response,
                    locale);

                return;
            }
        case i_amqp::connection_secure_id:
            {
                i_amqp::longstr_t challenge;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                challenge.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + challenge.size <= args_size);
                challenge.data = (void*) (args + offset);
                offset += challenge.size;

                callback->connection_secure (  channel_
,
                    challenge);

                return;
            }
        case i_amqp::connection_secure_ok_id:
            {
                i_amqp::longstr_t response;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                response.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + response.size <= args_size);
                response.data = (void*) (args + offset);
                offset += response.size;

                callback->connection_secure_ok (  channel_
,
                    response);

                return;
            }
        case i_amqp::connection_tune_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t channel_max = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t frame_max = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t heartbeat = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->connection_tune (  channel_
,
                    channel_max,
                    frame_max,
                    heartbeat);

                return;
            }
        case i_amqp::connection_tune_ok_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t channel_max = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t frame_max = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t heartbeat = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->connection_tune_ok (  channel_
,
                    channel_max,
                    frame_max,
                    heartbeat);

                return;
            }
        case i_amqp::connection_open_id:
            {
                i_amqp::shortstr_t virtual_host;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                virtual_host.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + virtual_host.size <= args_size);
                virtual_host.data = (char*) (args + offset);
                offset += virtual_host.size;
                i_amqp::shortstr_t reserved_1;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool reserved_2 = (args [offset] & (0x1 << 0)) != 0;

                callback->connection_open (  channel_
,
                    virtual_host,
                    reserved_1,
                    reserved_2);

                return;
            }
        case i_amqp::connection_open_ok_id:
            {
                i_amqp::shortstr_t reserved_1;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;

                callback->connection_open_ok (  channel_
,
                    reserved_1);

                return;
            }
        case i_amqp::connection_close_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t reply_text;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + reply_text.size <= args_size);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t class_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t method_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->connection_close (  channel_
,
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                return;
            }
        case i_amqp::connection_close_ok_id:
            {

                callback->connection_close_ok (  channel_
);

                return;
            }
        }
    case i_amqp::channel_id:
        switch (method_id) {
        case i_amqp::channel_open_id:
            {
                i_amqp::shortstr_t reserved_1;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;

                callback->channel_open (  channel_
,
                    reserved_1);

                return;
            }
        case i_amqp::channel_open_ok_id:
            {
                i_amqp::longstr_t reserved_1;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                reserved_1.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (void*) (args + offset);
                offset += reserved_1.size;

                callback->channel_open_ok (  channel_
,
                    reserved_1);

                return;
            }
        case i_amqp::channel_flow_id:
            {
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool active = (args [offset] & (0x1 << 0)) != 0;

                callback->channel_flow (  channel_
,
                    active);

                return;
            }
        case i_amqp::channel_flow_ok_id:
            {
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool active = (args [offset] & (0x1 << 0)) != 0;

                callback->channel_flow_ok (  channel_
,
                    active);

                return;
            }
        case i_amqp::channel_close_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t reply_text;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + reply_text.size <= args_size);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t class_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t method_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->channel_close (  channel_
,
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                return;
            }
        case i_amqp::channel_close_ok_id:
            {

                callback->channel_close_ok (  channel_
);

                return;
            }
        }
    case i_amqp::exchange_id:
        switch (method_id) {
        case i_amqp::exchange_declare_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t type;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                type.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + type.size <= args_size);
                type.data = (char*) (args + offset);
                offset += type.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool passive = (args [offset] & (0x1 << 0)) != 0;
                bool durable = (args [offset] & (0x1 << 1)) != 0;
                bool reserved_2 = (args [offset] & (0x1 << 2)) != 0;
                bool reserved_3 = (args [offset] & (0x1 << 3)) != 0;
                bool no_wait = (args [offset] & (0x1 << 4)) != 0;
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->exchange_declare (  channel_
,
                    reserved_1,
                    exchange,
                    type,
                    passive,
                    durable,
                    reserved_2,
                    reserved_3,
                    no_wait,
                    arguments);

                return;
            }
        case i_amqp::exchange_declare_ok_id:
            {

                callback->exchange_declare_ok (  channel_
);

                return;
            }
        case i_amqp::exchange_delete_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool if_unused = (args [offset] & (0x1 << 0)) != 0;
                bool no_wait = (args [offset] & (0x1 << 1)) != 0;

                callback->exchange_delete (  channel_
,
                    reserved_1,
                    exchange,
                    if_unused,
                    no_wait);

                return;
            }
        case i_amqp::exchange_delete_ok_id:
            {

                callback->exchange_delete_ok (  channel_
);

                return;
            }
        }
    case i_amqp::queue_id:
        switch (method_id) {
        case i_amqp::queue_declare_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool passive = (args [offset] & (0x1 << 0)) != 0;
                bool durable = (args [offset] & (0x1 << 1)) != 0;
                bool exclusive = (args [offset] & (0x1 << 2)) != 0;
                bool auto_delete = (args [offset] & (0x1 << 3)) != 0;
                bool no_wait = (args [offset] & (0x1 << 4)) != 0;
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->queue_declare (  channel_
,
                    reserved_1,
                    queue,
                    passive,
                    durable,
                    exclusive,
                    auto_delete,
                    no_wait,
                    arguments);

                return;
            }
        case i_amqp::queue_declare_ok_id:
            {
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t consumer_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->queue_declare_ok (  channel_
,
                    queue,
                    message_count,
                    consumer_count);

                return;
            }
        case i_amqp::queue_bind_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool no_wait = (args [offset] & (0x1 << 0)) != 0;
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->queue_bind (  channel_
,
                    reserved_1,
                    queue,
                    exchange,
                    routing_key,
                    no_wait,
                    arguments);

                return;
            }
        case i_amqp::queue_bind_ok_id:
            {

                callback->queue_bind_ok (  channel_
);

                return;
            }
        case i_amqp::queue_unbind_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->queue_unbind (  channel_
,
                    reserved_1,
                    queue,
                    exchange,
                    routing_key,
                    arguments);

                return;
            }
        case i_amqp::queue_unbind_ok_id:
            {

                callback->queue_unbind_ok (  channel_
);

                return;
            }
        case i_amqp::queue_purge_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool no_wait = (args [offset] & (0x1 << 0)) != 0;

                callback->queue_purge (  channel_
,
                    reserved_1,
                    queue,
                    no_wait);

                return;
            }
        case i_amqp::queue_purge_ok_id:
            {
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->queue_purge_ok (  channel_
,
                    message_count);

                return;
            }
        case i_amqp::queue_delete_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool if_unused = (args [offset] & (0x1 << 0)) != 0;
                bool if_empty = (args [offset] & (0x1 << 1)) != 0;
                bool no_wait = (args [offset] & (0x1 << 2)) != 0;

                callback->queue_delete (  channel_
,
                    reserved_1,
                    queue,
                    if_unused,
                    if_empty,
                    no_wait);

                return;
            }
        case i_amqp::queue_delete_ok_id:
            {
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->queue_delete_ok (  channel_
,
                    message_count);

                return;
            }
        }
    case i_amqp::basic_id:
        switch (method_id) {
        case i_amqp::basic_qos_id:
            {
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t prefetch_size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t prefetch_count = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool global = (args [offset] & (0x1 << 0)) != 0;

                callback->basic_qos (  channel_
,
                    prefetch_size,
                    prefetch_count,
                    global);

                return;
            }
        case i_amqp::basic_qos_ok_id:
            {

                callback->basic_qos_ok (  channel_
);

                return;
            }
        case i_amqp::basic_consume_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp::shortstr_t consumer_tag;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool no_local = (args [offset] & (0x1 << 0)) != 0;
                bool no_ack = (args [offset] & (0x1 << 1)) != 0;
                bool exclusive = (args [offset] & (0x1 << 2)) != 0;
                bool no_wait = (args [offset] & (0x1 << 3)) != 0;
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->basic_consume (  channel_
,
                    reserved_1,
                    queue,
                    consumer_tag,
                    no_local,
                    no_ack,
                    exclusive,
                    no_wait,
                    arguments);

                return;
            }
        case i_amqp::basic_consume_ok_id:
            {
                i_amqp::shortstr_t consumer_tag;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                callback->basic_consume_ok (  channel_
,
                    consumer_tag);

                return;
            }
        case i_amqp::basic_cancel_id:
            {
                i_amqp::shortstr_t consumer_tag;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool no_wait = (args [offset] & (0x1 << 0)) != 0;

                callback->basic_cancel (  channel_
,
                    consumer_tag,
                    no_wait);

                return;
            }
        case i_amqp::basic_cancel_ok_id:
            {
                i_amqp::shortstr_t consumer_tag;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                callback->basic_cancel_ok (  channel_
,
                    consumer_tag);

                return;
            }
        case i_amqp::basic_publish_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool mandatory = (args [offset] & (0x1 << 0)) != 0;
                bool immediate = (args [offset] & (0x1 << 1)) != 0;

                callback->basic_publish (  channel_
,
                    reserved_1,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate);

                return;
            }
        case i_amqp::basic_return_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t reply_text;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + reply_text.size <= args_size);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                callback->basic_return (  channel_
,
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp::basic_deliver_id:
            {
                i_amqp::shortstr_t consumer_tag;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                zmq_assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool redelivered = (args [offset] & (0x1 << 0)) != 0;
                offset += sizeof (uint8_t);
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                callback->basic_deliver (  channel_
,
                    consumer_tag,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp::basic_get_id:
            {
                zmq_assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool no_ack = (args [offset] & (0x1 << 0)) != 0;

                callback->basic_get (  channel_
,
                    reserved_1,
                    queue,
                    no_ack);

                return;
            }
        case i_amqp::basic_get_ok_id:
            {
                zmq_assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool redelivered = (args [offset] & (0x1 << 0)) != 0;
                offset += sizeof (uint8_t);
                i_amqp::shortstr_t exchange;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->basic_get_ok (  channel_
,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key,
                    message_count);

                return;
            }
        case i_amqp::basic_get_empty_id:
            {
                i_amqp::shortstr_t reserved_1;
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                zmq_assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;

                callback->basic_get_empty (  channel_
,
                    reserved_1);

                return;
            }
        case i_amqp::basic_ack_id:
            {
                zmq_assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool multiple = (args [offset] & (0x1 << 0)) != 0;

                callback->basic_ack (  channel_
,
                    delivery_tag,
                    multiple);

                return;
            }
        case i_amqp::basic_reject_id:
            {
                zmq_assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool requeue = (args [offset] & (0x1 << 0)) != 0;

                callback->basic_reject (  channel_
,
                    delivery_tag,
                    requeue);

                return;
            }
        case i_amqp::basic_recover_async_id:
            {
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool requeue = (args [offset] & (0x1 << 0)) != 0;

                callback->basic_recover_async (  channel_
,
                    requeue);

                return;
            }
        case i_amqp::basic_recover_id:
            {
                zmq_assert (offset + sizeof (uint8_t) <= args_size);
                bool requeue = (args [offset] & (0x1 << 0)) != 0;

                callback->basic_recover (  channel_
,
                    requeue);

                return;
            }
        case i_amqp::basic_recover_ok_id:
            {

                callback->basic_recover_ok (  channel_
);

                return;
            }
        }
    case i_amqp::tx_id:
        switch (method_id) {
        case i_amqp::tx_select_id:
            {

                callback->tx_select (  channel_
);

                return;
            }
        case i_amqp::tx_select_ok_id:
            {

                callback->tx_select_ok (  channel_
);

                return;
            }
        case i_amqp::tx_commit_id:
            {

                callback->tx_commit (  channel_
);

                return;
            }
        case i_amqp::tx_commit_ok_id:
            {

                callback->tx_commit_ok (  channel_
);

                return;
            }
        case i_amqp::tx_rollback_id:
            {

                callback->tx_rollback (  channel_
);

                return;
            }
        case i_amqp::tx_rollback_ok_id:
            {

                callback->tx_rollback_ok (  channel_
);

                return;
            }
        }
    }
}

void zmq::amqp_unmarshaller_t::get_field_table (unsigned char *args,
    size_t args_size, size_t offset, i_amqp::field_table_t &table_)
{
    table_.clear ();
    zmq_assert (offset + sizeof (uint32_t) <= args_size);
    uint32_t table_size = get_uint32 (args + offset);
    offset += sizeof (uint32_t);
    size_t pos = 0;
    while (pos != table_size) {

        //  Get field name
        zmq_assert (offset + sizeof (uint8_t) <= args_size);
        uint8_t field_name_string_size = get_uint8 (args + offset);
        offset += sizeof (uint8_t);
        zmq_assert (offset + field_name_string_size <= args_size);
        std::string field_name ((const char*) (args + offset),
            field_name_string_size);
        offset += field_name_string_size;

        //  Get field type
        zmq_assert (offset + sizeof (uint8_t) <= args_size);
        uint8_t field_type = get_uint8 (args + offset);
        offset += sizeof (uint8_t);

        //  Get field value
        std::string field_value;
        switch (field_type) {
        case 'S':
            {
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t field_value_size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                zmq_assert (offset + field_value_size <= args_size);
                field_value.assign ((const char*) (args + offset),
                    field_value_size);
                offset += field_value_size;
                pos += (sizeof (uint8_t) + field_name.size () + sizeof (uint8_t)
                    + sizeof (uint32_t) + field_value.size());
                break;
            }
        case 'I':
            {
                zmq_assert (offset + sizeof (uint32_t) <= args_size);
                uint16_t value = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                char buff [32];
                zmq_snprintf (buff, 32, "%ld", (long) value);
                field_value = buff;
                pos += (sizeof (uint8_t) + field_name.size () + sizeof (uint8_t)
                    + sizeof (uint32_t));
                break;
            }
        default:
            zmq_assert (false);
        }

        //  Push the field into field table       
        table_ [field_name] = field_value;
    }
}

#endif
