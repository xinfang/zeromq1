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

    This file is generated from amqp0-9.xml by amqp09.gsl using GSL/4.
*/

#include "amqp09_unmarshaller.hpp"
#include "wire.hpp"

void zmq::amqp09_unmarshaller_t::write (uint16_t class_id,
            uint16_t method_id, unsigned char *args, size_t args_size)
{
    size_t offset = 0;

    switch (class_id) {
    case i_amqp09::connection_id:
        switch (method_id) {
        case i_amqp09::connection_start_id:
            {
                uint8_t version_major = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                uint8_t version_minor = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                i_amqp09::field_table_t server_properties;
                get_field_table (args, args_size, offset, server_properties);
                i_amqp09::longstr_t mechanisms;
                mechanisms.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                mechanisms.data = (void*) (args + offset);
                offset += mechanisms.size;
                i_amqp09::longstr_t locales;
                locales.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                locales.data = (void*) (args + offset);
                offset += locales.size;

                amqp->connection_start (
                    version_major,
                    version_minor,
                    server_properties,
                    mechanisms,
                    locales);

                return;
            }
        case i_amqp09::connection_start_ok_id:
            {
                i_amqp09::field_table_t client_properties;
                get_field_table (args, args_size, offset, client_properties);
                i_amqp09::shortstr_t mechanism;
                mechanism.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                mechanism.data = (char*) (args + offset);
                offset += mechanism.size;
                i_amqp09::longstr_t response;
                response.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                response.data = (void*) (args + offset);
                offset += response.size;
                i_amqp09::shortstr_t locale;
                locale.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                locale.data = (char*) (args + offset);
                offset += locale.size;

                amqp->connection_start_ok (
                    client_properties,
                    mechanism,
                    response,
                    locale);

                return;
            }
        case i_amqp09::connection_secure_id:
            {
                i_amqp09::longstr_t challenge;
                challenge.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                challenge.data = (void*) (args + offset);
                offset += challenge.size;

                amqp->connection_secure (
                    challenge);

                return;
            }
        case i_amqp09::connection_secure_ok_id:
            {
                i_amqp09::longstr_t response;
                response.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                response.data = (void*) (args + offset);
                offset += response.size;

                amqp->connection_secure_ok (
                    response);

                return;
            }
        case i_amqp09::connection_tune_id:
            {
                uint16_t channel_max = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                uint32_t frame_max = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                uint16_t heartbeat = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                amqp->connection_tune (
                    channel_max,
                    frame_max,
                    heartbeat);

                return;
            }
        case i_amqp09::connection_tune_ok_id:
            {
                uint16_t channel_max = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                uint32_t frame_max = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                uint16_t heartbeat = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                amqp->connection_tune_ok (
                    channel_max,
                    frame_max,
                    heartbeat);

                return;
            }
        case i_amqp09::connection_open_id:
            {
                i_amqp09::shortstr_t virtual_host;
                virtual_host.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                virtual_host.data = (char*) (args + offset);
                offset += virtual_host.size;
                i_amqp09::shortstr_t capabilities;
                capabilities.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                capabilities.data = (char*) (args + offset);
                offset += capabilities.size;
                bool insist = args [offset] & (0x1 << 0);

                amqp->connection_open (
                    virtual_host,
                    capabilities,
                    insist);

                return;
            }
        case i_amqp09::connection_open_ok_id:
            {
                i_amqp09::shortstr_t known_hosts;
                known_hosts.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                known_hosts.data = (char*) (args + offset);
                offset += known_hosts.size;

                amqp->connection_open_ok (
                    known_hosts);

                return;
            }
        case i_amqp09::connection_redirect_id:
            {
                i_amqp09::shortstr_t host;
                host.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                host.data = (char*) (args + offset);
                offset += host.size;
                i_amqp09::shortstr_t known_hosts;
                known_hosts.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                known_hosts.data = (char*) (args + offset);
                offset += known_hosts.size;

                amqp->connection_redirect (
                    host,
                    known_hosts);

                return;
            }
        case i_amqp09::connection_close_id:
            {
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t reply_text;
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                uint16_t class_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                uint16_t method_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                amqp->connection_close (
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                return;
            }
        case i_amqp09::connection_close_ok_id:
            {

                amqp->connection_close_ok ();

                return;
            }
        }
    case i_amqp09::channel_id:
        switch (method_id) {
        case i_amqp09::channel_open_id:
            {
                i_amqp09::shortstr_t out_of_band;
                out_of_band.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                out_of_band.data = (char*) (args + offset);
                offset += out_of_band.size;

                amqp->channel_open (
                    out_of_band);

                return;
            }
        case i_amqp09::channel_open_ok_id:
            {
                i_amqp09::longstr_t channel_id;
                channel_id.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                channel_id.data = (void*) (args + offset);
                offset += channel_id.size;

                amqp->channel_open_ok (
                    channel_id);

                return;
            }
        case i_amqp09::channel_flow_id:
            {
                bool active = args [offset] & (0x1 << 0);

                amqp->channel_flow (
                    active);

                return;
            }
        case i_amqp09::channel_flow_ok_id:
            {
                bool active = args [offset] & (0x1 << 0);

                amqp->channel_flow_ok (
                    active);

                return;
            }
        case i_amqp09::channel_close_id:
            {
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t reply_text;
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                uint16_t class_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                uint16_t method_id = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                amqp->channel_close (
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                return;
            }
        case i_amqp09::channel_close_ok_id:
            {

                amqp->channel_close_ok ();

                return;
            }
        case i_amqp09::channel_resume_id:
            {
                i_amqp09::longstr_t channel_id;
                channel_id.size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                channel_id.data = (void*) (args + offset);
                offset += channel_id.size;

                amqp->channel_resume (
                    channel_id);

                return;
            }
        case i_amqp09::channel_ping_id:
            {

                amqp->channel_ping ();

                return;
            }
        case i_amqp09::channel_pong_id:
            {

                amqp->channel_pong ();

                return;
            }
        case i_amqp09::channel_ok_id:
            {

                amqp->channel_ok ();

                return;
            }
        }
    case i_amqp09::access_id:
        switch (method_id) {
        case i_amqp09::access_request_id:
            {
                i_amqp09::shortstr_t realm;
                realm.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                realm.data = (char*) (args + offset);
                offset += realm.size;
                bool exclusive = args [offset] & (0x1 << 0);
                bool passive = args [offset] & (0x1 << 1);
                bool active = args [offset] & (0x1 << 2);
                bool write = args [offset] & (0x1 << 3);
                bool read = args [offset] & (0x1 << 4);

                amqp->access_request (
                    realm,
                    exclusive,
                    passive,
                    active,
                    write,
                    read);

                return;
            }
        case i_amqp09::access_request_ok_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);

                amqp->access_request_ok (
                    ticket);

                return;
            }
        }
    case i_amqp09::exchange_id:
        switch (method_id) {
        case i_amqp09::exchange_declare_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t type;
                type.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                type.data = (char*) (args + offset);
                offset += type.size;
                bool passive = args [offset] & (0x1 << 0);
                bool durable = args [offset] & (0x1 << 1);
                bool auto_delete = args [offset] & (0x1 << 2);
                bool internal = args [offset] & (0x1 << 3);
                bool nowait = args [offset] & (0x1 << 4);
                offset += sizeof (uint8_t);
                i_amqp09::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                amqp->exchange_declare (
                    ticket,
                    exchange,
                    type,
                    passive,
                    durable,
                    auto_delete,
                    internal,
                    nowait,
                    arguments);

                return;
            }
        case i_amqp09::exchange_declare_ok_id:
            {

                amqp->exchange_declare_ok ();

                return;
            }
        case i_amqp09::exchange_delete_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                bool if_unused = args [offset] & (0x1 << 0);
                bool nowait = args [offset] & (0x1 << 1);

                amqp->exchange_delete (
                    ticket,
                    exchange,
                    if_unused,
                    nowait);

                return;
            }
        case i_amqp09::exchange_delete_ok_id:
            {

                amqp->exchange_delete_ok ();

                return;
            }
        }
    case i_amqp09::queue_id:
        switch (method_id) {
        case i_amqp09::queue_declare_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                bool passive = args [offset] & (0x1 << 0);
                bool durable = args [offset] & (0x1 << 1);
                bool exclusive = args [offset] & (0x1 << 2);
                bool auto_delete = args [offset] & (0x1 << 3);
                bool nowait = args [offset] & (0x1 << 4);
                offset += sizeof (uint8_t);
                i_amqp09::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                amqp->queue_declare (
                    ticket,
                    queue,
                    passive,
                    durable,
                    exclusive,
                    auto_delete,
                    nowait,
                    arguments);

                return;
            }
        case i_amqp09::queue_declare_ok_id:
            {
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                uint32_t consumer_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                amqp->queue_declare_ok (
                    queue,
                    message_count,
                    consumer_count);

                return;
            }
        case i_amqp09::queue_bind_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                bool nowait = args [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                i_amqp09::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                amqp->queue_bind (
                    ticket,
                    queue,
                    exchange,
                    routing_key,
                    nowait,
                    arguments);

                return;
            }
        case i_amqp09::queue_bind_ok_id:
            {

                amqp->queue_bind_ok ();

                return;
            }
        case i_amqp09::queue_unbind_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                i_amqp09::field_table_t arguments;
                get_field_table (args, args_size, offset, arguments);

                amqp->queue_unbind (
                    ticket,
                    queue,
                    exchange,
                    routing_key,
                    arguments);

                return;
            }
        case i_amqp09::queue_unbind_ok_id:
            {

                amqp->queue_unbind_ok ();

                return;
            }
        case i_amqp09::queue_purge_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                bool nowait = args [offset] & (0x1 << 0);

                amqp->queue_purge (
                    ticket,
                    queue,
                    nowait);

                return;
            }
        case i_amqp09::queue_purge_ok_id:
            {
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                amqp->queue_purge_ok (
                    message_count);

                return;
            }
        case i_amqp09::queue_delete_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                bool if_unused = args [offset] & (0x1 << 0);
                bool if_empty = args [offset] & (0x1 << 1);
                bool nowait = args [offset] & (0x1 << 2);

                amqp->queue_delete (
                    ticket,
                    queue,
                    if_unused,
                    if_empty,
                    nowait);

                return;
            }
        case i_amqp09::queue_delete_ok_id:
            {
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                amqp->queue_delete_ok (
                    message_count);

                return;
            }
        }
    case i_amqp09::basic_id:
        switch (method_id) {
        case i_amqp09::basic_qos_id:
            {
                uint32_t prefetch_size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                uint16_t prefetch_count = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                bool global = args [offset] & (0x1 << 0);

                amqp->basic_qos (
                    prefetch_size,
                    prefetch_count,
                    global);

                return;
            }
        case i_amqp09::basic_qos_ok_id:
            {

                amqp->basic_qos_ok ();

                return;
            }
        case i_amqp09::basic_consume_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                bool no_local = args [offset] & (0x1 << 0);
                bool no_ack = args [offset] & (0x1 << 1);
                bool exclusive = args [offset] & (0x1 << 2);
                bool nowait = args [offset] & (0x1 << 3);
                offset += sizeof (uint8_t);
                i_amqp09::field_table_t filter;
                get_field_table (args, args_size, offset, filter);

                amqp->basic_consume (
                    ticket,
                    queue,
                    consumer_tag,
                    no_local,
                    no_ack,
                    exclusive,
                    nowait,
                    filter);

                return;
            }
        case i_amqp09::basic_consume_ok_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                amqp->basic_consume_ok (
                    consumer_tag);

                return;
            }
        case i_amqp09::basic_cancel_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                bool nowait = args [offset] & (0x1 << 0);

                amqp->basic_cancel (
                    consumer_tag,
                    nowait);

                return;
            }
        case i_amqp09::basic_cancel_ok_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                amqp->basic_cancel_ok (
                    consumer_tag);

                return;
            }
        case i_amqp09::basic_publish_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                bool mandatory = args [offset] & (0x1 << 0);
                bool immediate = args [offset] & (0x1 << 1);

                amqp->basic_publish (
                    ticket,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate);

                return;
            }
        case i_amqp09::basic_return_id:
            {
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t reply_text;
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                amqp->basic_return (
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp09::basic_deliver_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                bool redelivered = args [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                amqp->basic_deliver (
                    consumer_tag,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp09::basic_get_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                bool no_ack = args [offset] & (0x1 << 0);

                amqp->basic_get (
                    ticket,
                    queue,
                    no_ack);

                return;
            }
        case i_amqp09::basic_get_ok_id:
            {
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                bool redelivered = args [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                uint32_t message_count = get_uint32 (args + offset);
                offset += sizeof (uint32_t);

                amqp->basic_get_ok (
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key,
                    message_count);

                return;
            }
        case i_amqp09::basic_get_empty_id:
            {
                i_amqp09::shortstr_t cluster_id;
                cluster_id.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                cluster_id.data = (char*) (args + offset);
                offset += cluster_id.size;

                amqp->basic_get_empty (
                    cluster_id);

                return;
            }
        case i_amqp09::basic_ack_id:
            {
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                bool multiple = args [offset] & (0x1 << 0);

                amqp->basic_ack (
                    delivery_tag,
                    multiple);

                return;
            }
        case i_amqp09::basic_reject_id:
            {
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                bool requeue = args [offset] & (0x1 << 0);

                amqp->basic_reject (
                    delivery_tag,
                    requeue);

                return;
            }
        case i_amqp09::basic_recover_id:
            {
                bool requeue = args [offset] & (0x1 << 0);

                amqp->basic_recover (
                    requeue);

                return;
            }
        }
    case i_amqp09::file_id:
        switch (method_id) {
        case i_amqp09::file_qos_id:
            {
                uint32_t prefetch_size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                uint16_t prefetch_count = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                bool global = args [offset] & (0x1 << 0);

                amqp->file_qos (
                    prefetch_size,
                    prefetch_count,
                    global);

                return;
            }
        case i_amqp09::file_qos_ok_id:
            {

                amqp->file_qos_ok ();

                return;
            }
        case i_amqp09::file_consume_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                bool no_local = args [offset] & (0x1 << 0);
                bool no_ack = args [offset] & (0x1 << 1);
                bool exclusive = args [offset] & (0x1 << 2);
                bool nowait = args [offset] & (0x1 << 3);
                offset += sizeof (uint8_t);
                i_amqp09::field_table_t filter;
                get_field_table (args, args_size, offset, filter);

                amqp->file_consume (
                    ticket,
                    queue,
                    consumer_tag,
                    no_local,
                    no_ack,
                    exclusive,
                    nowait,
                    filter);

                return;
            }
        case i_amqp09::file_consume_ok_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                amqp->file_consume_ok (
                    consumer_tag);

                return;
            }
        case i_amqp09::file_cancel_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                bool nowait = args [offset] & (0x1 << 0);

                amqp->file_cancel (
                    consumer_tag,
                    nowait);

                return;
            }
        case i_amqp09::file_cancel_ok_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                amqp->file_cancel_ok (
                    consumer_tag);

                return;
            }
        case i_amqp09::file_open_id:
            {
                i_amqp09::shortstr_t identifier;
                identifier.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                identifier.data = (char*) (args + offset);
                offset += identifier.size;
                uint64_t content_size = get_uint64 (args + offset);
                offset += sizeof (uint64_t);

                amqp->file_open (
                    identifier,
                    content_size);

                return;
            }
        case i_amqp09::file_open_ok_id:
            {
                uint64_t staged_size = get_uint64 (args + offset);
                offset += sizeof (uint64_t);

                amqp->file_open_ok (
                    staged_size);

                return;
            }
        case i_amqp09::file_stage_id:
            {

                amqp->file_stage ();

                return;
            }
        case i_amqp09::file_publish_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                bool mandatory = args [offset] & (0x1 << 0);
                bool immediate = args [offset] & (0x1 << 1);
                offset += sizeof (uint8_t);
                i_amqp09::shortstr_t identifier;
                identifier.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                identifier.data = (char*) (args + offset);
                offset += identifier.size;

                amqp->file_publish (
                    ticket,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate,
                    identifier);

                return;
            }
        case i_amqp09::file_return_id:
            {
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t reply_text;
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                amqp->file_return (
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp09::file_deliver_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                bool redelivered = args [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                i_amqp09::shortstr_t identifier;
                identifier.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                identifier.data = (char*) (args + offset);
                offset += identifier.size;

                amqp->file_deliver (
                    consumer_tag,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key,
                    identifier);

                return;
            }
        case i_amqp09::file_ack_id:
            {
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                bool multiple = args [offset] & (0x1 << 0);

                amqp->file_ack (
                    delivery_tag,
                    multiple);

                return;
            }
        case i_amqp09::file_reject_id:
            {
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                bool requeue = args [offset] & (0x1 << 0);

                amqp->file_reject (
                    delivery_tag,
                    requeue);

                return;
            }
        }
    case i_amqp09::stream_id:
        switch (method_id) {
        case i_amqp09::stream_qos_id:
            {
                uint32_t prefetch_size = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                uint16_t prefetch_count = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                uint32_t consume_rate = get_uint32 (args + offset);
                offset += sizeof (uint32_t);
                bool global = args [offset] & (0x1 << 0);

                amqp->stream_qos (
                    prefetch_size,
                    prefetch_count,
                    consume_rate,
                    global);

                return;
            }
        case i_amqp09::stream_qos_ok_id:
            {

                amqp->stream_qos_ok ();

                return;
            }
        case i_amqp09::stream_consume_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                bool no_local = args [offset] & (0x1 << 0);
                bool exclusive = args [offset] & (0x1 << 1);
                bool nowait = args [offset] & (0x1 << 2);
                offset += sizeof (uint8_t);
                i_amqp09::field_table_t filter;
                get_field_table (args, args_size, offset, filter);

                amqp->stream_consume (
                    ticket,
                    queue,
                    consumer_tag,
                    no_local,
                    exclusive,
                    nowait,
                    filter);

                return;
            }
        case i_amqp09::stream_consume_ok_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                amqp->stream_consume_ok (
                    consumer_tag);

                return;
            }
        case i_amqp09::stream_cancel_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                bool nowait = args [offset] & (0x1 << 0);

                amqp->stream_cancel (
                    consumer_tag,
                    nowait);

                return;
            }
        case i_amqp09::stream_cancel_ok_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;

                amqp->stream_cancel_ok (
                    consumer_tag);

                return;
            }
        case i_amqp09::stream_publish_id:
            {
                uint16_t ticket = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;
                bool mandatory = args [offset] & (0x1 << 0);
                bool immediate = args [offset] & (0x1 << 1);

                amqp->stream_publish (
                    ticket,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate);

                return;
            }
        case i_amqp09::stream_return_id:
            {
                uint16_t reply_code = get_uint16 (args + offset);
                offset += sizeof (uint16_t);
                i_amqp09::shortstr_t reply_text;
                reply_text.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (args + offset);
                offset += reply_text.size;
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t routing_key;
                routing_key.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (args + offset);
                offset += routing_key.size;

                amqp->stream_return (
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                return;
            }
        case i_amqp09::stream_deliver_id:
            {
                i_amqp09::shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (args + offset);
                offset += consumer_tag.size;
                uint64_t delivery_tag = get_uint64 (args + offset);
                offset += sizeof (uint64_t);
                i_amqp09::shortstr_t exchange;
                exchange.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (args + offset);
                offset += exchange.size;
                i_amqp09::shortstr_t queue;
                queue.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (args + offset);
                offset += queue.size;

                amqp->stream_deliver (
                    consumer_tag,
                    delivery_tag,
                    exchange,
                    queue);

                return;
            }
        }
    case i_amqp09::tx_id:
        switch (method_id) {
        case i_amqp09::tx_select_id:
            {

                amqp->tx_select ();

                return;
            }
        case i_amqp09::tx_select_ok_id:
            {

                amqp->tx_select_ok ();

                return;
            }
        case i_amqp09::tx_commit_id:
            {

                amqp->tx_commit ();

                return;
            }
        case i_amqp09::tx_commit_ok_id:
            {

                amqp->tx_commit_ok ();

                return;
            }
        case i_amqp09::tx_rollback_id:
            {

                amqp->tx_rollback ();

                return;
            }
        case i_amqp09::tx_rollback_ok_id:
            {

                amqp->tx_rollback_ok ();

                return;
            }
        }
    case i_amqp09::dtx_id:
        switch (method_id) {
        case i_amqp09::dtx_select_id:
            {

                amqp->dtx_select ();

                return;
            }
        case i_amqp09::dtx_select_ok_id:
            {

                amqp->dtx_select_ok ();

                return;
            }
        case i_amqp09::dtx_start_id:
            {
                i_amqp09::shortstr_t dtx_identifier;
                dtx_identifier.size = get_uint8 (args + offset);
                offset += sizeof (uint8_t);
                dtx_identifier.data = (char*) (args + offset);
                offset += dtx_identifier.size;

                amqp->dtx_start (
                    dtx_identifier);

                return;
            }
        case i_amqp09::dtx_start_ok_id:
            {

                amqp->dtx_start_ok ();

                return;
            }
        }
    case i_amqp09::tunnel_id:
        switch (method_id) {
        case i_amqp09::tunnel_request_id:
            {
                i_amqp09::field_table_t meta_data;
                get_field_table (args, args_size, offset, meta_data);

                amqp->tunnel_request (
                    meta_data);

                return;
            }
        }
    }
}

void zmq::amqp09_unmarshaller_t::get_field_table (unsigned char *args,
    size_t args_size, size_t offset, i_amqp09::field_table_t &table_)
{
    table_.clear ();
    uint32_t table_size = get_uint32 (args + offset);
    offset += sizeof (uint32_t);
    size_t pos = 0;
    while (pos != table_size) {

        //  Get field name
        uint8_t field_name_string_size = get_uint8 (args + offset);
        offset += sizeof (uint8_t);
        std::string field_name ((const char*) (args + offset),
            field_name_string_size);
        offset += field_name_string_size;

        //  Get field type
        uint8_t field_type = get_uint8 (args + offset);
        offset += sizeof (uint8_t);
        assert (field_type == 'S');

        //  Get field value
        std::string field_value;
        uint32_t field_value_size = get_uint32 (args + offset);
        offset += sizeof (uint32_t);
        field_value.assign ((const char*) (args + offset), field_value_size);
        offset += field_value_size;
        pos += (sizeof (uint8_t) + field_name.size () +
            sizeof (uint8_t) + sizeof (uint32_t) + field_value.size());

        //  Push the field into field table       
        table_ [field_name] = field_value;
    }
}
