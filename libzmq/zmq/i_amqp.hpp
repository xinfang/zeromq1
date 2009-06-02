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

#ifndef __ZMQ_I_AMQP_HPP_INCLUDED__
#define __ZMQ_I_AMQP_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <assert.h>
#include <map>
#include <string>
#include <cstring> 

#include <zmq/stdint.hpp>

namespace zmq
{

    //  Virtual interface from AMQP (0-9) commands. AMQP marshaller uses
    //  the interface to accept AMQP commands and convert them into binary
    //  represenatation. AMQP unmarshaller uses the interface to fire
    //  commands extracted from the binary representation.

    struct i_amqp
    {

        //  AMQP constants
        enum {
            frame_method = 1,
            frame_header = 2,
            frame_body = 3,
            frame_heartbeat = 8,
            frame_min_size = 4096,
            frame_end = 206,
            reply_success = 200,
            content_too_large = 311,
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
            unexpected_frame = 505,
            resource_error = 506,
            not_allowed = 530,
            not_implemented = 540,
            internal_error = 541,
            connection_id = 10,
            channel_id = 20,
            exchange_id = 40,
            queue_id = 50,
            basic_id = 60,
            tx_id = 90,
            connection_start_id = 10,
            connection_start_ok_id = 11,
            connection_secure_id = 20,
            connection_secure_ok_id = 21,
            connection_tune_id = 30,
            connection_tune_ok_id = 31,
            connection_open_id = 40,
            connection_open_ok_id = 41,
            connection_close_id = 50,
            connection_close_ok_id = 51,
            channel_open_id = 10,
            channel_open_ok_id = 11,
            channel_flow_id = 20,
            channel_flow_ok_id = 21,
            channel_close_id = 40,
            channel_close_ok_id = 41,
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
            basic_recover_async_id = 100,
            basic_recover_id = 110,
            basic_recover_ok_id = 111,
            tx_select_id = 10,
            tx_select_ok_id = 11,
            tx_commit_id = 20,
            tx_commit_ok_id = 21,
            tx_rollback_id = 30,
            tx_rollback_ok_id = 31,
            end_amqp_constants
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
        virtual ~i_amqp () {};

        inline virtual void connection_start (
            uint16_t /* channel_ */,
            uint8_t /* version_major_ */,
            uint8_t /* version_minor_ */,
            const i_amqp::field_table_t &/* server_properties_ */,
            const i_amqp::longstr_t /* mechanisms_ */,
            const i_amqp::longstr_t /* locales_ */)
        {
            assert (false);
        }

        inline virtual void connection_start_ok (
            uint16_t /* channel_ */,
            const i_amqp::field_table_t &/* client_properties_ */,
            const i_amqp::shortstr_t /* mechanism_ */,
            const i_amqp::longstr_t /* response_ */,
            const i_amqp::shortstr_t /* locale_ */)
        {
            assert (false);
        }

        inline virtual void connection_secure (
            uint16_t /* channel_ */,
            const i_amqp::longstr_t /* challenge_ */)
        {
            assert (false);
        }

        inline virtual void connection_secure_ok (
            uint16_t /* channel_ */,
            const i_amqp::longstr_t /* response_ */)
        {
            assert (false);
        }

        inline virtual void connection_tune (
            uint16_t /* channel_ */,
            uint16_t /* channel_max_ */,
            uint32_t /* frame_max_ */,
            uint16_t /* heartbeat_ */)
        {
            assert (false);
        }

        inline virtual void connection_tune_ok (
            uint16_t /* channel_ */,
            uint16_t /* channel_max_ */,
            uint32_t /* frame_max_ */,
            uint16_t /* heartbeat_ */)
        {
            assert (false);
        }

        inline virtual void connection_open (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* virtual_host_ */,
            const i_amqp::shortstr_t /* reserved_1_ */,
            bool /* reserved_2_ */)
        {
            assert (false);
        }

        inline virtual void connection_open_ok (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* reserved_1_ */)
        {
            assert (false);
        }

        inline virtual void connection_close (
            uint16_t /* channel_ */,
            uint16_t /* reply_code_ */,
            const i_amqp::shortstr_t /* reply_text_ */,
            uint16_t /* class_id_ */,
            uint16_t /* method_id_ */)
        {
            assert (false);
        }

        inline virtual void connection_close_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void channel_open (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* reserved_1_ */)
        {
            assert (false);
        }

        inline virtual void channel_open_ok (
            uint16_t /* channel_ */,
            const i_amqp::longstr_t /* reserved_1_ */)
        {
            assert (false);
        }

        inline virtual void channel_flow (
            uint16_t /* channel_ */,
            bool /* active_ */)
        {
            assert (false);
        }

        inline virtual void channel_flow_ok (
            uint16_t /* channel_ */,
            bool /* active_ */)
        {
            assert (false);
        }

        inline virtual void channel_close (
            uint16_t /* channel_ */,
            uint16_t /* reply_code_ */,
            const i_amqp::shortstr_t /* reply_text_ */,
            uint16_t /* class_id_ */,
            uint16_t /* method_id_ */)
        {
            assert (false);
        }

        inline virtual void channel_close_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void exchange_declare (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            const i_amqp::shortstr_t /* type_ */,
            bool /* passive_ */,
            bool /* durable_ */,
            bool /* reserved_2_ */,
            bool /* reserved_3_ */,
            bool /* no_wait_ */,
            const i_amqp::field_table_t &/* arguments_ */)
        {
            assert (false);
        }

        inline virtual void exchange_declare_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void exchange_delete (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            bool /* if_unused_ */,
            bool /* no_wait_ */)
        {
            assert (false);
        }

        inline virtual void exchange_delete_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void queue_declare (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* queue_ */,
            bool /* passive_ */,
            bool /* durable_ */,
            bool /* exclusive_ */,
            bool /* auto_delete_ */,
            bool /* no_wait_ */,
            const i_amqp::field_table_t &/* arguments_ */)
        {
            assert (false);
        }

        inline virtual void queue_declare_ok (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* queue_ */,
            uint32_t /* message_count_ */,
            uint32_t /* consumer_count_ */)
        {
            assert (false);
        }

        inline virtual void queue_bind (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* queue_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            const i_amqp::shortstr_t /* routing_key_ */,
            bool /* no_wait_ */,
            const i_amqp::field_table_t &/* arguments_ */)
        {
            assert (false);
        }

        inline virtual void queue_bind_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void queue_unbind (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* queue_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            const i_amqp::shortstr_t /* routing_key_ */,
            const i_amqp::field_table_t &/* arguments_ */)
        {
            assert (false);
        }

        inline virtual void queue_unbind_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void queue_purge (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* queue_ */,
            bool /* no_wait_ */)
        {
            assert (false);
        }

        inline virtual void queue_purge_ok (
            uint16_t /* channel_ */,
            uint32_t /* message_count_ */)
        {
            assert (false);
        }

        inline virtual void queue_delete (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* queue_ */,
            bool /* if_unused_ */,
            bool /* if_empty_ */,
            bool /* no_wait_ */)
        {
            assert (false);
        }

        inline virtual void queue_delete_ok (
            uint16_t /* channel_ */,
            uint32_t /* message_count_ */)
        {
            assert (false);
        }

        inline virtual void basic_qos (
            uint16_t /* channel_ */,
            uint32_t /* prefetch_size_ */,
            uint16_t /* prefetch_count_ */,
            bool /* global_ */)
        {
            assert (false);
        }

        inline virtual void basic_qos_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void basic_consume (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* queue_ */,
            const i_amqp::shortstr_t /* consumer_tag_ */,
            bool /* no_local_ */,
            bool /* no_ack_ */,
            bool /* exclusive_ */,
            bool /* no_wait_ */,
            const i_amqp::field_table_t &/* arguments_ */)
        {
            assert (false);
        }

        inline virtual void basic_consume_ok (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* consumer_tag_ */)
        {
            assert (false);
        }

        inline virtual void basic_cancel (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* consumer_tag_ */,
            bool /* no_wait_ */)
        {
            assert (false);
        }

        inline virtual void basic_cancel_ok (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* consumer_tag_ */)
        {
            assert (false);
        }

        inline virtual void basic_publish (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            const i_amqp::shortstr_t /* routing_key_ */,
            bool /* mandatory_ */,
            bool /* immediate_ */)
        {
            assert (false);
        }

        inline virtual void basic_return (
            uint16_t /* channel_ */,
            uint16_t /* reply_code_ */,
            const i_amqp::shortstr_t /* reply_text_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            const i_amqp::shortstr_t /* routing_key_ */)
        {
            assert (false);
        }

        inline virtual void basic_deliver (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* consumer_tag_ */,
            uint64_t /* delivery_tag_ */,
            bool /* redelivered_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            const i_amqp::shortstr_t /* routing_key_ */)
        {
            assert (false);
        }

        inline virtual void basic_get (
            uint16_t /* channel_ */,
            uint16_t /* reserved_1_ */,
            const i_amqp::shortstr_t /* queue_ */,
            bool /* no_ack_ */)
        {
            assert (false);
        }

        inline virtual void basic_get_ok (
            uint16_t /* channel_ */,
            uint64_t /* delivery_tag_ */,
            bool /* redelivered_ */,
            const i_amqp::shortstr_t /* exchange_ */,
            const i_amqp::shortstr_t /* routing_key_ */,
            uint32_t /* message_count_ */)
        {
            assert (false);
        }

        inline virtual void basic_get_empty (
            uint16_t /* channel_ */,
            const i_amqp::shortstr_t /* reserved_1_ */)
        {
            assert (false);
        }

        inline virtual void basic_ack (
            uint16_t /* channel_ */,
            uint64_t /* delivery_tag_ */,
            bool /* multiple_ */)
        {
            assert (false);
        }

        inline virtual void basic_reject (
            uint16_t /* channel_ */,
            uint64_t /* delivery_tag_ */,
            bool /* requeue_ */)
        {
            assert (false);
        }

        inline virtual void basic_recover_async (
            uint16_t /* channel_ */,
            bool /* requeue_ */)
        {
            assert (false);
        }

        inline virtual void basic_recover (
            uint16_t /* channel_ */,
            bool /* requeue_ */)
        {
            assert (false);
        }

        inline virtual void basic_recover_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void tx_select (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void tx_select_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void tx_commit (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void tx_commit_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void tx_rollback (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

        inline virtual void tx_rollback_ok (
            uint16_t /* channel_ */)
        {
            assert (false);
        }

    };

}

#endif

#endif
