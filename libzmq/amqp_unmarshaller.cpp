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

void zmq::amqp_unmarshaller_t::write (uint16_t class_id,
            uint16_t method_id, unsigned char *args, size_t args_size)
{
    size_t offset = 0;

    switch (class_id) {
    case i_amqp::connection_id:
        switch (method_id) {
        case i_amqp::connection_start_id:
            {
                assert (offset + sizeof (uint8_t) <= args_size);
                uint8_t version_major = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + sizeof (uint8_t) <= args_size);
                uint8_t version_minor = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                i_amqp::field_table_t server_properties;
                get_field_table (args, args_size, offset, server_properties);
                i_amqp::longstr_t mechanisms;
                assert (offset + sizeof (uint32_t) <= args_size);
                mechanisms.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + mechanisms.size <= args_size);
                mechanisms.data = (void*) (args + offset);
                offset += mechanisms.size;
                i_amqp::longstr_t locales;
                assert (offset + sizeof (uint32_t) <= args_size);
                locales.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + locales.size <= args_size);
                locales.data = (void*) (args + offset);
                offset += locales.size;

                callback->connection_start (
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
                assert (offset + sizeof (uint8_t) <= args_size);
                mechanism.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + mechanism.size <= args_size);
                mechanism.data = (char*) (args + offset);
                offset += mechanism.size;
                i_amqp::longstr_t response;
                assert (offset + sizeof (uint32_t) <= args_size);
                response.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + response.size <= args_size);
                response.data = (void*) (args + offset);
                offset += response.size;
                i_amqp::shortstr_t locale;
                assert (offset + sizeof (uint8_t) <= args_size);
                locale.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + locale.size <= args_size);
                locale.data = (char*) (args + offset);
                offset += locale.size;

                callback->connection_start_ok (
                    client_properties,
                    mechanism,
                    response,
                    locale);

                return;
            }
        case i_amqp::connection_secure_id:
            {
                i_amqp::longstr_t challenge;
                assert (offset + sizeof (uint32_t) <= args_size);
                challenge.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + challenge.size <= args_size);
                challenge.data = (void*) (args + offset);
                offset += challenge.size;

                callback->connection_secure (
                    challenge);

                return;
            }
        case i_amqp::connection_secure_ok_id:
            {
                i_amqp::longstr_t response;
                assert (offset + sizeof (uint32_t) <= args_size);
                response.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + response.size <= args_size);
                response.data = (void*) (args + offset);
                offset += response.size;

                callback->connection_secure_ok (
                    response);

                return;
            }
        case i_amqp::connection_tune_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t channel_max = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t frame_max = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t heartbeat = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->connection_tune (
                    channel_max,
                    frame_max,
                    heartbeat);

                return;
            }
        case i_amqp::connection_tune_ok_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t channel_max = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t frame_max = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t heartbeat = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->connection_tune_ok (
                    channel_max,
                    frame_max,
                    heartbeat);

                return;
            }
        case i_amqp::connection_open_id:
            {
                i_amqp::shortstr_t virtual_host;
                assert (offset + sizeof (uint8_t) <= args_size);
                virtual_host.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + virtual_host.size <= args_size);
                virtual_host.data = (char*) (args + offset);
                offset += virtual_host.size;
                i_amqp::shortstr_t reserved_1;
                assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool reserved_2 = args [offset] & (0x1 << 0);

                callback->connection_open (
                    virtual_host,
                    reserved_1,
                    reserved_2);

                return;
            }
        case i_amqp::connection_open_ok_id:
            {
                i_amqp::shortstr_t reserved_1;
                assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;

                callback->connection_open_ok (
                    reserved_1);

                return;
            }
        case i_amqp::connection_close_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t reply_text;
                assert (offset + sizeof (uint8_t) <= args_size);
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + reply_text.size <= args_size);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t class_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t method_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->connection_close (
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                return;
            }
        case i_amqp::connection_close_ok_id:
            {

                callback->connection_close_ok ();

                return;
            }
        }
    case i_amqp::channel_id:
        switch (method_id) {
        case i_amqp::channel_open_id:
            {
                i_amqp::shortstr_t reserved_1;
                assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;

                callback->channel_open (
                    reserved_1);

                return;
            }
        case i_amqp::channel_open_ok_id:
            {
                i_amqp::longstr_t reserved_1;
                assert (offset + sizeof (uint32_t) <= args_size);
                reserved_1.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (void*) (args + offset);
                offset += reserved_1.size;

                callback->channel_open_ok (
                    reserved_1);

                return;
            }
        case i_amqp::channel_flow_id:
            {
                assert (offset + sizeof (uint8_t) <= args_size);
                bool active = args [offset] & (0x1 << 0);

                callback->channel_flow (
                    active);

                return;
            }
        case i_amqp::channel_flow_ok_id:
            {
                assert (offset + sizeof (uint8_t) <= args_size);
                bool active = args [offset] & (0x1 << 0);

                callback->channel_flow_ok (
                    active);

                return;
            }
        case i_amqp::channel_close_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t reply_text;
                assert (offset + sizeof (uint8_t) <= args_size);
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + reply_text.size <= args_size);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t class_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t method_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                callback->channel_close (
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                return;
            }
        case i_amqp::channel_close_ok_id:
            {

                callback->channel_close_ok ();

                return;
            }
        }
    case i_amqp::exchange_id:
        switch (method_id) {
        case i_amqp::exchange_declare_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t type;
                assert (offset + sizeof (uint8_t) <= args_size);
                type.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + type.size <= args_size);
                type.data = (char*) (args + offset);
                offset += type.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool passive = args [offset] & (0x1 << 0);
                bool durable = args [offset] & (0x1 << 1);
                bool reserved_2 = args [offset] & (0x1 << 2);
                bool reserved_3 = args [offset] & (0x1 << 3);
                bool no_wait = args [offset] & (0x1 << 4);
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->exchange_declare (
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

                callback->exchange_declare_ok ();

                return;
            }
        case i_amqp::exchange_delete_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool if_unused = args [offset] & (0x1 << 0);
                bool no_wait = args [offset] & (0x1 << 1);

                callback->exchange_delete (
                    reserved_1,
                    exchange,
                    if_unused,
                    no_wait);

                return;
            }
        case i_amqp::exchange_delete_ok_id:
            {

                callback->exchange_delete_ok ();

                return;
            }
        }
    case i_amqp::queue_id:
        switch (method_id) {
        case i_amqp::queue_declare_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool passive = args [offset] & (0x1 << 0);
                bool durable = args [offset] & (0x1 << 1);
                bool exclusive = args [offset] & (0x1 << 2);
                bool auto_delete = args [offset] & (0x1 << 3);
                bool no_wait = args [offset] & (0x1 << 4);
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->queue_declare (
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
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t consumer_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->queue_declare_ok (
                    queue,
                    message_count,
                    consumer_count);

                return;
            }
        case i_amqp::queue_bind_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool no_wait = args [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->queue_bind (
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

                callback->queue_bind_ok ();

                return;
            }
        case i_amqp::queue_unbind_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->queue_unbind (
                    reserved_1,
                    queue,
                    exchange,
                    routing_key,
                    arguments);

                return;
            }
        case i_amqp::queue_unbind_ok_id:
            {

                callback->queue_unbind_ok ();

                return;
            }
        case i_amqp::queue_purge_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool no_wait = args [offset] & (0x1 << 0);

                callback->queue_purge (
                    reserved_1,
                    queue,
                    no_wait);

                return;
            }
        case i_amqp::queue_purge_ok_id:
            {
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->queue_purge_ok (
                    message_count);

                return;
            }
        case i_amqp::queue_delete_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool if_unused = args [offset] & (0x1 << 0);
                bool if_empty = args [offset] & (0x1 << 1);
                bool no_wait = args [offset] & (0x1 << 2);

                callback->queue_delete (
                    reserved_1,
                    queue,
                    if_unused,
                    if_empty,
                    no_wait);

                return;
            }
        case i_amqp::queue_delete_ok_id:
            {
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->queue_delete_ok (
                    message_count);

                return;
            }
        }
    case i_amqp::basic_id:
        switch (method_id) {
        case i_amqp::basic_qos_id:
            {
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t prefetch_size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t prefetch_count = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                assert (offset + sizeof (uint8_t) <= args_size);
                bool global = args [offset] & (0x1 << 0);

                callback->basic_qos (
                    prefetch_size,
                    prefetch_count,
                    global);

                return;
            }
        case i_amqp::basic_qos_ok_id:
            {

                callback->basic_qos_ok ();

                return;
            }
        case i_amqp::basic_consume_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp::shortstr_t consumer_tag;
                assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool no_local = args [offset] & (0x1 << 0);
                bool no_ack = args [offset] & (0x1 << 1);
                bool exclusive = args [offset] & (0x1 << 2);
                bool no_wait = args [offset] & (0x1 << 3);
                offset += sizeof (uint8_t);
                i_amqp::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                callback->basic_consume (
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
                assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                callback->basic_consume_ok (
                    consumer_tag);

                return;
            }
        case i_amqp::basic_cancel_id:
            {
                i_amqp::shortstr_t consumer_tag;
                assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool no_wait = args [offset] & (0x1 << 0);

                callback->basic_cancel (
                    consumer_tag,
                    no_wait);

                return;
            }
        case i_amqp::basic_cancel_ok_id:
            {
                i_amqp::shortstr_t consumer_tag;
                assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                callback->basic_cancel_ok (
                    consumer_tag);

                return;
            }
        case i_amqp::basic_publish_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool mandatory = args [offset] & (0x1 << 0);
                bool immediate = args [offset] & (0x1 << 1);

                callback->basic_publish (
                    reserved_1,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate);

                return;
            }
        case i_amqp::basic_return_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t reply_text;
                assert (offset + sizeof (uint8_t) <= args_size);
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + reply_text.size <= args_size);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                callback->basic_return (
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp::basic_deliver_id:
            {
                i_amqp::shortstr_t consumer_tag;
                assert (offset + sizeof (uint8_t) <= args_size);
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + consumer_tag.size <= args_size);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                assert (offset + sizeof (uint8_t) <= args_size);
                bool redelivered = args [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                callback->basic_deliver (
                    consumer_tag,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp::basic_get_id:
            {
                assert (offset + sizeof (uint16_t) <= args_size);
                uint16_t reserved_1 = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp::shortstr_t queue;
                assert (offset + sizeof (uint8_t) <= args_size);
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + queue.size <= args_size);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                assert (offset + sizeof (uint8_t) <= args_size);
                bool no_ack = args [offset] & (0x1 << 0);

                callback->basic_get (
                    reserved_1,
                    queue,
                    no_ack);

                return;
            }
        case i_amqp::basic_get_ok_id:
            {
                assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                assert (offset + sizeof (uint8_t) <= args_size);
                bool redelivered = args [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                i_amqp::shortstr_t exchange;
                assert (offset + sizeof (uint8_t) <= args_size);
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + exchange.size <= args_size);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp::shortstr_t routing_key;
                assert (offset + sizeof (uint8_t) <= args_size);
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + routing_key.size <= args_size);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                callback->basic_get_ok (
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
                assert (offset + sizeof (uint8_t) <= args_size);
                reserved_1.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                assert (offset + reserved_1.size <= args_size);
                reserved_1.data = (char*) (args + offset);
                offset += reserved_1.size;

                callback->basic_get_empty (
                    reserved_1);

                return;
            }
        case i_amqp::basic_ack_id:
            {
                assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                assert (offset + sizeof (uint8_t) <= args_size);
                bool multiple = args [offset] & (0x1 << 0);

                callback->basic_ack (
                    delivery_tag,
                    multiple);

                return;
            }
        case i_amqp::basic_reject_id:
            {
                assert (offset + sizeof (uint64_t) <= args_size);
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                assert (offset + sizeof (uint8_t) <= args_size);
                bool requeue = args [offset] & (0x1 << 0);

                callback->basic_reject (
                    delivery_tag,
                    requeue);

                return;
            }
        case i_amqp::basic_recover_async_id:
            {
                assert (offset + sizeof (uint8_t) <= args_size);
                bool requeue = args [offset] & (0x1 << 0);

                callback->basic_recover_async (
                    requeue);

                return;
            }
        case i_amqp::basic_recover_id:
            {
                assert (offset + sizeof (uint8_t) <= args_size);
                bool requeue = args [offset] & (0x1 << 0);

                callback->basic_recover (
                    requeue);

                return;
            }
        case i_amqp::basic_recover_ok_id:
            {

                callback->basic_recover_ok ();

                return;
            }
        }
    case i_amqp::tx_id:
        switch (method_id) {
        case i_amqp::tx_select_id:
            {

                callback->tx_select ();

                return;
            }
        case i_amqp::tx_select_ok_id:
            {

                callback->tx_select_ok ();

                return;
            }
        case i_amqp::tx_commit_id:
            {

                callback->tx_commit ();

                return;
            }
        case i_amqp::tx_commit_ok_id:
            {

                callback->tx_commit_ok ();

                return;
            }
        case i_amqp::tx_rollback_id:
            {

                callback->tx_rollback ();

                return;
            }
        case i_amqp::tx_rollback_ok_id:
            {

                callback->tx_rollback_ok ();

                return;
            }
        }
    }
}

void zmq::amqp_unmarshaller_t::get_field_table (unsigned char *args,
    size_t args_size, size_t offset, i_amqp::field_table_t &table_)
{
    table_.clear ();
    assert (offset + sizeof (uint32_t) <= args_size);
    uint32_t table_size = get_uint32 (args + offset);
    offset += sizeof (uint32_t);
    size_t pos = 0;
    while (pos != table_size) {

        //  Get field name
        assert (offset + sizeof (uint8_t) <= args_size);
        uint8_t field_name_string_size = get_uint8 (args + offset);
        offset += sizeof (uint8_t);
        assert (offset + field_name_string_size <= args_size);
        std::string field_name ((const char*) (args + offset),
            field_name_string_size);
        offset += field_name_string_size;

        //  Get field type
        assert (offset + sizeof (uint8_t) <= args_size);
        uint8_t field_type = get_uint8 (args + offset);
        offset += sizeof (uint8_t);

        //  Get field value
        std::string field_value;
        switch (field_type) {
        case 'S':
            {
                assert (offset + sizeof (uint32_t) <= args_size);
                uint32_t field_value_size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                assert (offset + field_value_size <= args_size);
                field_value.assign ((const char*) (args + offset),
                    field_value_size);
                offset += field_value_size;
                pos += (sizeof (uint8_t) + field_name.size () + sizeof (uint8_t)
                    + sizeof (uint32_t) + field_value.size());
                break;
            }
        case 'I':
            {
                assert (offset + sizeof (uint32_t) <= args_size);
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
            assert (false);
        }

        //  Push the field into field table       
        table_ [field_name] = field_value;
    }
}

#endif
