/*
    Copyright (c) 2007 FastMQ Inc.

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

    This file is generated from amqp0-9.xml by amqp09_base.gsl using GSL/4.
*/

#ifndef __ZMQ_AMQP09_BASE_HPP_INCLUDED__
#define __ZMQ_AMQP09_BASE_HPP_INCLUDED__

#include <cstring>
#include <string>
#include <map>
#include <assert.h>
#include <arpa/inet.h>

#include "tcp_socket.hpp"
#include "wire.hpp"

namespace zmq
{

    class amqp09_base_t : public tcp_socket_t
    {
    public:

        struct shortstr_t
        {
            inline shortstr_t () :
                data (NULL),
                size (0)
            {
            }

            inline shortstr_t (const char *data_) :
                data (data_)
            {
                size_t len = strlen (data);
                assert (len <= 0xff);
                size = len;
            }

            const char *data;
            uint8_t size;
        };

        struct longstr_t
        {
            inline longstr_t () :
                data (NULL),
                size (0)
            {
            }

            inline longstr_t (const void *data_, uint32_t size_) :
                data (data_),
                size (size_)
            {
            }

            const void *data;
            uint32_t size;
        };

        typedef std::map<std::string, std::string> field_table_t;

        amqp09_base_t (bool listen, const char *address, uint16_t port);

    protected:

        enum {
            amqp_frame_method = 1,
            amqp_frame_header = 2,
            amqp_frame_body = 3,
            amqp_frame_oob_method = 4,
            amqp_frame_oob_header = 5,
            amqp_frame_oob_body = 6,
            amqp_frame_trace = 7,
            amqp_frame_heartbeat = 8,
            amqp_frame_min_size = 4096,
            amqp_frame_end = 206,
            amqp_reply_success = 200,
            amqp_not_delivered = 310,
            amqp_content_too_large = 311,
            amqp_no_route = 312,
            amqp_no_consumers = 313,
            amqp_connection_forced = 320,
            amqp_invalid_path = 402,
            amqp_access_refused = 403,
            amqp_not_found = 404,
            amqp_resource_locked = 405,
            amqp_precondition_failed = 406,
            amqp_frame_error = 501,
            amqp_syntax_error = 502,
            amqp_command_invalid = 503,
            amqp_channel_error = 504,
            amqp_resource_error = 506,
            amqp_not_allowed = 530,
            amqp_not_implemented = 540,
            amqp_internal_error = 541
        };

        enum {
            amqp_connection = 10,
            amqp_channel = 20,
            amqp_access = 30,
            amqp_exchange = 40,
            amqp_queue = 50,
            amqp_basic = 60,
            amqp_file = 70,
            amqp_stream = 80,
            amqp_tx = 90,
            amqp_dtx = 100,
            amqp_tunnel = 110,
            amqp_message = 120
        };

        enum {
            amqp_connection_start = 10,
            amqp_connection_start_ok = 11,
            amqp_connection_secure = 20,
            amqp_connection_secure_ok = 21,
            amqp_connection_tune = 30,
            amqp_connection_tune_ok = 31,
            amqp_connection_open = 40,
            amqp_connection_open_ok = 41,
            amqp_connection_redirect = 42,
            amqp_connection_close = 50,
            amqp_connection_close_ok = 51,
            amqp_channel_open = 10,
            amqp_channel_open_ok = 11,
            amqp_channel_flow = 20,
            amqp_channel_flow_ok = 21,
            amqp_channel_close = 40,
            amqp_channel_close_ok = 41,
            amqp_channel_resume = 50,
            amqp_channel_ping = 60,
            amqp_channel_pong = 70,
            amqp_channel_ok = 80,
            amqp_access_request = 10,
            amqp_access_request_ok = 11,
            amqp_exchange_declare = 10,
            amqp_exchange_declare_ok = 11,
            amqp_exchange_delete = 20,
            amqp_exchange_delete_ok = 21,
            amqp_queue_declare = 10,
            amqp_queue_declare_ok = 11,
            amqp_queue_bind = 20,
            amqp_queue_bind_ok = 21,
            amqp_queue_unbind = 50,
            amqp_queue_unbind_ok = 51,
            amqp_queue_purge = 30,
            amqp_queue_purge_ok = 31,
            amqp_queue_delete = 40,
            amqp_queue_delete_ok = 41,
            amqp_basic_qos = 10,
            amqp_basic_qos_ok = 11,
            amqp_basic_consume = 20,
            amqp_basic_consume_ok = 21,
            amqp_basic_cancel = 30,
            amqp_basic_cancel_ok = 31,
            amqp_basic_publish = 40,
            amqp_basic_return = 50,
            amqp_basic_deliver = 60,
            amqp_basic_get = 70,
            amqp_basic_get_ok = 71,
            amqp_basic_get_empty = 72,
            amqp_basic_ack = 80,
            amqp_basic_reject = 90,
            amqp_basic_recover = 100,
            amqp_file_qos = 10,
            amqp_file_qos_ok = 11,
            amqp_file_consume = 20,
            amqp_file_consume_ok = 21,
            amqp_file_cancel = 30,
            amqp_file_cancel_ok = 31,
            amqp_file_open = 40,
            amqp_file_open_ok = 41,
            amqp_file_stage = 50,
            amqp_file_publish = 60,
            amqp_file_return = 70,
            amqp_file_deliver = 80,
            amqp_file_ack = 90,
            amqp_file_reject = 100,
            amqp_stream_qos = 10,
            amqp_stream_qos_ok = 11,
            amqp_stream_consume = 20,
            amqp_stream_consume_ok = 21,
            amqp_stream_cancel = 30,
            amqp_stream_cancel_ok = 31,
            amqp_stream_publish = 40,
            amqp_stream_return = 50,
            amqp_stream_deliver = 60,
            amqp_tx_select = 10,
            amqp_tx_select_ok = 11,
            amqp_tx_commit = 20,
            amqp_tx_commit_ok = 21,
            amqp_tx_rollback = 30,
            amqp_tx_rollback_ok = 31,
            amqp_dtx_select = 10,
            amqp_dtx_select_ok = 11,
            amqp_dtx_start = 20,
            amqp_dtx_start_ok = 21,
            amqp_tunnel_request = 10,
            amqp_message_transfer = 10,
            amqp_message_consume = 20,
            amqp_message_cancel = 30,
            amqp_message_get = 40,
            amqp_message_recover = 50,
            amqp_message_open = 60,
            amqp_message_close = 70,
            amqp_message_append = 80,
            amqp_message_checkpoint = 90,
            amqp_message_resume = 100,
            amqp_message_qos = 110,
            amqp_message_ok = 500,
            amqp_message_empty = 510,
            amqp_message_reject = 520,
            amqp_message_offset = 530,
        };

        void receive_protocol_header ();

        void send_protocol_header ();

        virtual void unexpected () = 0;

        virtual void connection_start (
            uint16_t channel_,
            uint8_t version_major_,
            uint8_t version_minor_,
            const field_table_t &server_properties_,
            const longstr_t mechanisms_,
            const longstr_t locales_);

        virtual void send_connection_start (
            uint16_t channel_,
            uint8_t version_major_,
            uint8_t version_minor_,
            const field_table_t &server_properties_,
            const longstr_t mechanisms_,
            const longstr_t locales_);

        virtual void connection_start_ok (
            uint16_t channel_,
            const field_table_t &client_properties_,
            const shortstr_t mechanism_,
            const longstr_t response_,
            const shortstr_t locale_);

        virtual void send_connection_start_ok (
            uint16_t channel_,
            const field_table_t &client_properties_,
            const shortstr_t mechanism_,
            const longstr_t response_,
            const shortstr_t locale_);

        virtual void connection_secure (
            uint16_t channel_,
            const longstr_t challenge_);

        virtual void send_connection_secure (
            uint16_t channel_,
            const longstr_t challenge_);

        virtual void connection_secure_ok (
            uint16_t channel_,
            const longstr_t response_);

        virtual void send_connection_secure_ok (
            uint16_t channel_,
            const longstr_t response_);

        virtual void connection_tune (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        virtual void send_connection_tune (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        virtual void connection_tune_ok (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        virtual void send_connection_tune_ok (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        virtual void connection_open (
            uint16_t channel_,
            const shortstr_t virtual_host_,
            const shortstr_t capabilities_,
            bool insist_);

        virtual void send_connection_open (
            uint16_t channel_,
            const shortstr_t virtual_host_,
            const shortstr_t capabilities_,
            bool insist_);

        virtual void connection_open_ok (
            uint16_t channel_,
            const shortstr_t known_hosts_);

        virtual void send_connection_open_ok (
            uint16_t channel_,
            const shortstr_t known_hosts_);

        virtual void connection_redirect (
            uint16_t channel_,
            const shortstr_t host_,
            const shortstr_t known_hosts_);

        virtual void send_connection_redirect (
            uint16_t channel_,
            const shortstr_t host_,
            const shortstr_t known_hosts_);

        virtual void connection_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        virtual void send_connection_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        virtual void connection_close_ok (
            uint16_t channel_);

        virtual void send_connection_close_ok (
            uint16_t channel_);

        virtual void channel_open (
            uint16_t channel_,
            const shortstr_t out_of_band_);

        virtual void send_channel_open (
            uint16_t channel_,
            const shortstr_t out_of_band_);

        virtual void channel_open_ok (
            uint16_t channel_,
            const longstr_t channel_id_);

        virtual void send_channel_open_ok (
            uint16_t channel_,
            const longstr_t channel_id_);

        virtual void channel_flow (
            uint16_t channel_,
            bool active_);

        virtual void send_channel_flow (
            uint16_t channel_,
            bool active_);

        virtual void channel_flow_ok (
            uint16_t channel_,
            bool active_);

        virtual void send_channel_flow_ok (
            uint16_t channel_,
            bool active_);

        virtual void channel_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        virtual void send_channel_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        virtual void channel_close_ok (
            uint16_t channel_);

        virtual void send_channel_close_ok (
            uint16_t channel_);

        virtual void channel_resume (
            uint16_t channel_,
            const longstr_t channel_id_);

        virtual void send_channel_resume (
            uint16_t channel_,
            const longstr_t channel_id_);

        virtual void channel_ping (
            uint16_t channel_);

        virtual void send_channel_ping (
            uint16_t channel_);

        virtual void channel_pong (
            uint16_t channel_);

        virtual void send_channel_pong (
            uint16_t channel_);

        virtual void channel_ok (
            uint16_t channel_);

        virtual void send_channel_ok (
            uint16_t channel_);

        virtual void access_request (
            uint16_t channel_,
            const shortstr_t realm_,
            bool exclusive_,
            bool passive_,
            bool active_,
            bool write_,
            bool read_);

        virtual void send_access_request (
            uint16_t channel_,
            const shortstr_t realm_,
            bool exclusive_,
            bool passive_,
            bool active_,
            bool write_,
            bool read_);

        virtual void access_request_ok (
            uint16_t channel_,
            uint16_t ticket_);

        virtual void send_access_request_ok (
            uint16_t channel_,
            uint16_t ticket_);

        virtual void exchange_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t type_,
            bool passive_,
            bool durable_,
            bool auto_delete_,
            bool internal_,
            bool nowait_,
            const field_table_t &arguments_);

        virtual void send_exchange_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t type_,
            bool passive_,
            bool durable_,
            bool auto_delete_,
            bool internal_,
            bool nowait_,
            const field_table_t &arguments_);

        virtual void exchange_declare_ok (
            uint16_t channel_);

        virtual void send_exchange_declare_ok (
            uint16_t channel_);

        virtual void exchange_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            bool if_unused_,
            bool nowait_);

        virtual void send_exchange_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            bool if_unused_,
            bool nowait_);

        virtual void exchange_delete_ok (
            uint16_t channel_);

        virtual void send_exchange_delete_ok (
            uint16_t channel_);

        virtual void queue_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const field_table_t &arguments_);

        virtual void send_queue_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const field_table_t &arguments_);

        virtual void queue_declare_ok (
            uint16_t channel_,
            const shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_);

        virtual void send_queue_declare_ok (
            uint16_t channel_,
            const shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_);

        virtual void queue_bind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool nowait_,
            const field_table_t &arguments_);

        virtual void send_queue_bind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool nowait_,
            const field_table_t &arguments_);

        virtual void queue_bind_ok (
            uint16_t channel_);

        virtual void send_queue_bind_ok (
            uint16_t channel_);

        virtual void queue_unbind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const field_table_t &arguments_);

        virtual void send_queue_unbind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const field_table_t &arguments_);

        virtual void queue_unbind_ok (
            uint16_t channel_);

        virtual void send_queue_unbind_ok (
            uint16_t channel_);

        virtual void queue_purge (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool nowait_);

        virtual void send_queue_purge (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool nowait_);

        virtual void queue_purge_ok (
            uint16_t channel_,
            uint32_t message_count_);

        virtual void send_queue_purge_ok (
            uint16_t channel_,
            uint32_t message_count_);

        virtual void queue_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool nowait_);

        virtual void send_queue_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool nowait_);

        virtual void queue_delete_ok (
            uint16_t channel_,
            uint32_t message_count_);

        virtual void send_queue_delete_ok (
            uint16_t channel_,
            uint32_t message_count_);

        virtual void basic_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_);

        virtual void send_basic_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_);

        virtual void basic_qos_ok (
            uint16_t channel_);

        virtual void send_basic_qos_ok (
            uint16_t channel_);

        virtual void basic_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_);

        virtual void send_basic_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_);

        virtual void basic_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void send_basic_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void basic_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_);

        virtual void send_basic_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_);

        virtual void basic_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void send_basic_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void basic_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_);

        virtual void send_basic_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_);

        virtual void basic_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void send_basic_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void basic_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void send_basic_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void basic_get (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool no_ack_);

        virtual void send_basic_get (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool no_ack_);

        virtual void basic_get_ok (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            uint32_t message_count_);

        virtual void send_basic_get_ok (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            uint32_t message_count_);

        virtual void basic_get_empty (
            uint16_t channel_,
            const shortstr_t cluster_id_);

        virtual void send_basic_get_empty (
            uint16_t channel_,
            const shortstr_t cluster_id_);

        virtual void basic_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_);

        virtual void send_basic_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_);

        virtual void basic_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_);

        virtual void send_basic_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_);

        virtual void basic_recover (
            uint16_t channel_,
            bool requeue_);

        virtual void send_basic_recover (
            uint16_t channel_,
            bool requeue_);

        virtual void file_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_);

        virtual void send_file_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_);

        virtual void file_qos_ok (
            uint16_t channel_);

        virtual void send_file_qos_ok (
            uint16_t channel_);

        virtual void file_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_);

        virtual void send_file_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_);

        virtual void file_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void send_file_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void file_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_);

        virtual void send_file_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_);

        virtual void file_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void send_file_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void file_open (
            uint16_t channel_,
            const shortstr_t identifier_,
            uint64_t content_size_);

        virtual void send_file_open (
            uint16_t channel_,
            const shortstr_t identifier_,
            uint64_t content_size_);

        virtual void file_open_ok (
            uint16_t channel_,
            uint64_t staged_size_);

        virtual void send_file_open_ok (
            uint16_t channel_,
            uint64_t staged_size_);

        virtual void file_stage (
            uint16_t channel_);

        virtual void send_file_stage (
            uint16_t channel_);

        virtual void file_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_,
            const shortstr_t identifier_);

        virtual void send_file_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_,
            const shortstr_t identifier_);

        virtual void file_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void send_file_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void file_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const shortstr_t identifier_);

        virtual void send_file_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const shortstr_t identifier_);

        virtual void file_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_);

        virtual void send_file_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_);

        virtual void file_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_);

        virtual void send_file_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_);

        virtual void stream_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            uint32_t consume_rate_,
            bool global_);

        virtual void send_stream_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            uint32_t consume_rate_,
            bool global_);

        virtual void stream_qos_ok (
            uint16_t channel_);

        virtual void send_stream_qos_ok (
            uint16_t channel_);

        virtual void stream_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_);

        virtual void send_stream_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_);

        virtual void stream_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void send_stream_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void stream_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_);

        virtual void send_stream_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_);

        virtual void stream_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void send_stream_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_);

        virtual void stream_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_);

        virtual void send_stream_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_);

        virtual void stream_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void send_stream_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_);

        virtual void stream_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            const shortstr_t exchange_,
            const shortstr_t queue_);

        virtual void send_stream_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            const shortstr_t exchange_,
            const shortstr_t queue_);

        virtual void tx_select (
            uint16_t channel_);

        virtual void send_tx_select (
            uint16_t channel_);

        virtual void tx_select_ok (
            uint16_t channel_);

        virtual void send_tx_select_ok (
            uint16_t channel_);

        virtual void tx_commit (
            uint16_t channel_);

        virtual void send_tx_commit (
            uint16_t channel_);

        virtual void tx_commit_ok (
            uint16_t channel_);

        virtual void send_tx_commit_ok (
            uint16_t channel_);

        virtual void tx_rollback (
            uint16_t channel_);

        virtual void send_tx_rollback (
            uint16_t channel_);

        virtual void tx_rollback_ok (
            uint16_t channel_);

        virtual void send_tx_rollback_ok (
            uint16_t channel_);

        virtual void dtx_select (
            uint16_t channel_);

        virtual void send_dtx_select (
            uint16_t channel_);

        virtual void dtx_select_ok (
            uint16_t channel_);

        virtual void send_dtx_select_ok (
            uint16_t channel_);

        virtual void dtx_start (
            uint16_t channel_,
            const shortstr_t dtx_identifier_);

        virtual void send_dtx_start (
            uint16_t channel_,
            const shortstr_t dtx_identifier_);

        virtual void dtx_start_ok (
            uint16_t channel_);

        virtual void send_dtx_start_ok (
            uint16_t channel_);

        virtual void tunnel_request (
            uint16_t channel_,
            const field_table_t &meta_data_);

        virtual void send_tunnel_request (
            uint16_t channel_,
            const field_table_t &meta_data_);

        void dispatch ();

    private:

        void get_field_table (field_table_t &table_);
        void put_field_table (const field_table_t &table_);

        unsigned char buf [amqp_frame_min_size];
        size_t offset;
    };

}

#endif
