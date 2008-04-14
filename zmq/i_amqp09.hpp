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

#ifndef __ZMQ_I_AMQP09_HPP_INCLUDED__
#define __ZMQ_I_AMQP09_HPP_INCLUDED__

#include <assert.h>
#include <map>
#include <string>

namespace zmq
{

    //  Virtual interface from AMQP (0-9) commands. AMQP marshaller uses
    //  the interface to accept AMQP commands and convert them into binary
    //  represenatation. AMQP unmarshaller uses the interface to fire
    //  commands extracted from the binary representation.

    struct i_amqp09
    {

        //  AMQP constants
        enum {
            frame_method = 1,
            frame_header = 2,
            frame_body = 3,
            frame_oob_method = 4,
            frame_oob_header = 5,
            frame_oob_body = 6,
            frame_trace = 7,
            frame_heartbeat = 8,
            frame_min_size = 4096,
            frame_end = 206,
            reply_success = 200,
            not_delivered = 310,
            content_too_large = 311,
            no_route = 312,
            no_consumers = 313,
            connection_forced = 320,
            invalid_path = 402,
            access_refused = 403,
            not_found = 404,
            resource_locked = 405,
            precondition_failed = 406,
            frame_error = 501,
            syntax_error = 502,
            command_invalid = 503,
            channel_error = 504,
            resource_error = 506,
            not_allowed = 530,
            not_implemented = 540,
            internal_error = 541,
            connection_id = 10,
            channel_id = 20,
            access_id = 30,
            exchange_id = 40,
            queue_id = 50,
            basic_id = 60,
            file_id = 70,
            stream_id = 80,
            tx_id = 90,
            dtx_id = 100,
            tunnel_id = 110,
            connection_start_id = 10,
            connection_start_ok_id = 11,
            connection_secure_id = 20,
            connection_secure_ok_id = 21,
            connection_tune_id = 30,
            connection_tune_ok_id = 31,
            connection_open_id = 40,
            connection_open_ok_id = 41,
            connection_redirect_id = 42,
            connection_close_id = 50,
            connection_close_ok_id = 51,
            channel_open_id = 10,
            channel_open_ok_id = 11,
            channel_flow_id = 20,
            channel_flow_ok_id = 21,
            channel_close_id = 40,
            channel_close_ok_id = 41,
            channel_resume_id = 50,
            channel_ping_id = 60,
            channel_pong_id = 70,
            channel_ok_id = 80,
            access_request_id = 10,
            access_request_ok_id = 11,
            exchange_declare_id = 10,
            exchange_declare_ok_id = 11,
            exchange_delete_id = 20,
            exchange_delete_ok_id = 21,
            queue_declare_id = 10,
            queue_declare_ok_id = 11,
            queue_bind_id = 20,
            queue_bind_ok_id = 21,
            queue_unbind_id = 50,
            queue_unbind_ok_id = 51,
            queue_purge_id = 30,
            queue_purge_ok_id = 31,
            queue_delete_id = 40,
            queue_delete_ok_id = 41,
            basic_qos_id = 10,
            basic_qos_ok_id = 11,
            basic_consume_id = 20,
            basic_consume_ok_id = 21,
            basic_cancel_id = 30,
            basic_cancel_ok_id = 31,
            basic_publish_id = 40,
            basic_return_id = 50,
            basic_deliver_id = 60,
            basic_get_id = 70,
            basic_get_ok_id = 71,
            basic_get_empty_id = 72,
            basic_ack_id = 80,
            basic_reject_id = 90,
            basic_recover_id = 100,
            file_qos_id = 10,
            file_qos_ok_id = 11,
            file_consume_id = 20,
            file_consume_ok_id = 21,
            file_cancel_id = 30,
            file_cancel_ok_id = 31,
            file_open_id = 40,
            file_open_ok_id = 41,
            file_stage_id = 50,
            file_publish_id = 60,
            file_return_id = 70,
            file_deliver_id = 80,
            file_ack_id = 90,
            file_reject_id = 100,
            stream_qos_id = 10,
            stream_qos_ok_id = 11,
            stream_consume_id = 20,
            stream_consume_ok_id = 21,
            stream_cancel_id = 30,
            stream_cancel_ok_id = 31,
            stream_publish_id = 40,
            stream_return_id = 50,
            stream_deliver_id = 60,
            tx_select_id = 10,
            tx_select_ok_id = 11,
            tx_commit_id = 20,
            tx_commit_ok_id = 21,
            tx_rollback_id = 30,
            tx_rollback_ok_id = 31,
            dtx_select_id = 10,
            dtx_select_ok_id = 11,
            dtx_start_id = 20,
            dtx_start_ok_id = 21,
            tunnel_request_id = 10,
        };

        //  Wrapper class for AMQP shortstr datatype
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

        //  Wrapper class for AMQP long str datatype
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

        //  Wrapper class for AMQP field table datatype
        //  It is a STL map where the field name is the key and field value
        //  is the mapped value.
        typedef std::map<std::string, std::string> field_table_t;

        //  The destructor shouldn't be virtual, however, not defining it as
        //  such results in compiler warnings with some compilers.
        virtual ~i_amqp09 () {};

        inline virtual void connection_start (
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp09::field_table_t &server_properties_,
            const i_amqp09::longstr_t mechanisms_,
            const i_amqp09::longstr_t locales_)
        {
            unexpected ();
        }

        inline virtual void connection_start_ok (
            const i_amqp09::field_table_t &client_properties_,
            const i_amqp09::shortstr_t mechanism_,
            const i_amqp09::longstr_t response_,
            const i_amqp09::shortstr_t locale_)
        {
            unexpected ();
        }

        inline virtual void connection_secure (
            const i_amqp09::longstr_t challenge_)
        {
            unexpected ();
        }

        inline virtual void connection_secure_ok (
            const i_amqp09::longstr_t response_)
        {
            unexpected ();
        }

        inline virtual void connection_tune (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
        {
            unexpected ();
        }

        inline virtual void connection_tune_ok (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
        {
            unexpected ();
        }

        inline virtual void connection_open (
            const i_amqp09::shortstr_t virtual_host_,
            const i_amqp09::shortstr_t capabilities_,
            bool insist_)
        {
            unexpected ();
        }

        inline virtual void connection_open_ok (
            const i_amqp09::shortstr_t known_hosts_)
        {
            unexpected ();
        }

        inline virtual void connection_redirect (
            const i_amqp09::shortstr_t host_,
            const i_amqp09::shortstr_t known_hosts_)
        {
            unexpected ();
        }

        inline virtual void connection_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
        {
            unexpected ();
        }

        inline virtual void connection_close_ok ()
        {
            unexpected ();
        }

        inline virtual void channel_open (
            const i_amqp09::shortstr_t out_of_band_)
        {
            unexpected ();
        }

        inline virtual void channel_open_ok (
            const i_amqp09::longstr_t channel_id_)
        {
            unexpected ();
        }

        inline virtual void channel_flow (
            bool active_)
        {
            unexpected ();
        }

        inline virtual void channel_flow_ok (
            bool active_)
        {
            unexpected ();
        }

        inline virtual void channel_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
        {
            unexpected ();
        }

        inline virtual void channel_close_ok ()
        {
            unexpected ();
        }

        inline virtual void channel_resume (
            const i_amqp09::longstr_t channel_id_)
        {
            unexpected ();
        }

        inline virtual void channel_ping ()
        {
            unexpected ();
        }

        inline virtual void channel_pong ()
        {
            unexpected ();
        }

        inline virtual void channel_ok ()
        {
            unexpected ();
        }

        inline virtual void access_request (
            const i_amqp09::shortstr_t realm_,
            bool exclusive_,
            bool passive_,
            bool active_,
            bool write_,
            bool read_)
        {
            unexpected ();
        }

        inline virtual void access_request_ok (
            uint16_t ticket_)
        {
            unexpected ();
        }

        inline virtual void exchange_declare (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t type_,
            bool passive_,
            bool durable_,
            bool auto_delete_,
            bool internal_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_)
        {
            unexpected ();
        }

        inline virtual void exchange_declare_ok ()
        {
            unexpected ();
        }

        inline virtual void exchange_delete (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            bool if_unused_,
            bool nowait_)
        {
            unexpected ();
        }

        inline virtual void exchange_delete_ok ()
        {
            unexpected ();
        }

        inline virtual void queue_declare (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_)
        {
            unexpected ();
        }

        inline virtual void queue_declare_ok (
            const i_amqp09::shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_)
        {
            unexpected ();
        }

        inline virtual void queue_bind (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_)
        {
            unexpected ();
        }

        inline virtual void queue_bind_ok ()
        {
            unexpected ();
        }

        inline virtual void queue_unbind (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            const i_amqp09::field_table_t &arguments_)
        {
            unexpected ();
        }

        inline virtual void queue_unbind_ok ()
        {
            unexpected ();
        }

        inline virtual void queue_purge (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool nowait_)
        {
            unexpected ();
        }

        inline virtual void queue_purge_ok (
            uint32_t message_count_)
        {
            unexpected ();
        }

        inline virtual void queue_delete (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool nowait_)
        {
            unexpected ();
        }

        inline virtual void queue_delete_ok (
            uint32_t message_count_)
        {
            unexpected ();
        }

        inline virtual void basic_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
        {
            unexpected ();
        }

        inline virtual void basic_qos_ok ()
        {
            unexpected ();
        }

        inline virtual void basic_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_)
        {
            unexpected ();
        }

        inline virtual void basic_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_)
        {
            unexpected ();
        }

        inline virtual void basic_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_)
        {
            unexpected ();
        }

        inline virtual void basic_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_)
        {
            unexpected ();
        }

        inline virtual void basic_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
        {
            unexpected ();
        }

        inline virtual void basic_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
        {
            unexpected ();
        }

        inline virtual void basic_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
        {
            unexpected ();
        }

        inline virtual void basic_get (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool no_ack_)
        {
            unexpected ();
        }

        inline virtual void basic_get_ok (
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            uint32_t message_count_)
        {
            unexpected ();
        }

        inline virtual void basic_get_empty (
            const i_amqp09::shortstr_t cluster_id_)
        {
            unexpected ();
        }

        inline virtual void basic_ack (
            uint64_t delivery_tag_,
            bool multiple_)
        {
            unexpected ();
        }

        inline virtual void basic_reject (
            uint64_t delivery_tag_,
            bool requeue_)
        {
            unexpected ();
        }

        inline virtual void basic_recover (
            bool requeue_)
        {
            unexpected ();
        }

        inline virtual void file_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
        {
            unexpected ();
        }

        inline virtual void file_qos_ok ()
        {
            unexpected ();
        }

        inline virtual void file_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_)
        {
            unexpected ();
        }

        inline virtual void file_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_)
        {
            unexpected ();
        }

        inline virtual void file_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_)
        {
            unexpected ();
        }

        inline virtual void file_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_)
        {
            unexpected ();
        }

        inline virtual void file_open (
            const i_amqp09::shortstr_t identifier_,
            uint64_t content_size_)
        {
            unexpected ();
        }

        inline virtual void file_open_ok (
            uint64_t staged_size_)
        {
            unexpected ();
        }

        inline virtual void file_stage ()
        {
            unexpected ();
        }

        inline virtual void file_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_,
            const i_amqp09::shortstr_t identifier_)
        {
            unexpected ();
        }

        inline virtual void file_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
        {
            unexpected ();
        }

        inline virtual void file_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            const i_amqp09::shortstr_t identifier_)
        {
            unexpected ();
        }

        inline virtual void file_ack (
            uint64_t delivery_tag_,
            bool multiple_)
        {
            unexpected ();
        }

        inline virtual void file_reject (
            uint64_t delivery_tag_,
            bool requeue_)
        {
            unexpected ();
        }

        inline virtual void stream_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            uint32_t consume_rate_,
            bool global_)
        {
            unexpected ();
        }

        inline virtual void stream_qos_ok ()
        {
            unexpected ();
        }

        inline virtual void stream_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_)
        {
            unexpected ();
        }

        inline virtual void stream_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_)
        {
            unexpected ();
        }

        inline virtual void stream_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_)
        {
            unexpected ();
        }

        inline virtual void stream_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_)
        {
            unexpected ();
        }

        inline virtual void stream_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
        {
            unexpected ();
        }

        inline virtual void stream_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
        {
            unexpected ();
        }

        inline virtual void stream_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t queue_)
        {
            unexpected ();
        }

        inline virtual void tx_select ()
        {
            unexpected ();
        }

        inline virtual void tx_select_ok ()
        {
            unexpected ();
        }

        inline virtual void tx_commit ()
        {
            unexpected ();
        }

        inline virtual void tx_commit_ok ()
        {
            unexpected ();
        }

        inline virtual void tx_rollback ()
        {
            unexpected ();
        }

        inline virtual void tx_rollback_ok ()
        {
            unexpected ();
        }

        inline virtual void dtx_select ()
        {
            unexpected ();
        }

        inline virtual void dtx_select_ok ()
        {
            unexpected ();
        }

        inline virtual void dtx_start (
            const i_amqp09::shortstr_t dtx_identifier_)
        {
            unexpected ();
        }

        inline virtual void dtx_start_ok ()
        {
            unexpected ();
        }

        inline virtual void tunnel_request (
            const i_amqp09::field_table_t &meta_data_)
        {
            unexpected ();
        }

        //  Method to handle unexpected commands. Any command that is not
        //  overloaded in the interface implementation (derived class)
        //  is mapped to 'unexpected'.
        virtual void unexpected () = 0;
    };

}

#endif
