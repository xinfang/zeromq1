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

#ifndef __ZMQ_AMQP09_MARSHALLER_HPP_INCLUDED__
#define __ZMQ_AMQP09_MARSHALLER_HPP_INCLUDED__

#include <queue>

#include "i_amqp09.hpp"
#include "i_signaler.hpp"

namespace zmq
{

    //  Marshaller class converts calls to i_amqp09 interface into corresponding
    //  binary representation. Binary representations are stored in a queue to
    //  be retrieved by AMQP encoder when needed.

    class amqp09_marshaller_t : public i_amqp09
    {
    public:

        //  Structure to hold binary representation of AMQP command.
        //  'args' member (parameters of the command in binary format is to
        //  be allocated using malloc and deallocated using free.
        struct command_t
        {
            uint16_t class_id;
            uint16_t method_id;
            unsigned char *args;
            size_t args_size;
        };

        //  Creates marshaller object. The signaler suplied will be used to
        //  notify about command arriving it the marshaller's command queue.
        //
        //  TODO: This is actually a synchronous signal passed to the engine
        //  object. It shouldn't use i_signaler interface which is intended
        //  to be used for asynchronous signals.
        amqp09_marshaller_t (i_signaler *signaler_);
        ~amqp09_marshaller_t ();

        void connection_start (
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp09::field_table_t &server_properties_,
            const i_amqp09::longstr_t mechanisms_,
            const i_amqp09::longstr_t locales_);

        void connection_start_ok (
            const i_amqp09::field_table_t &client_properties_,
            const i_amqp09::shortstr_t mechanism_,
            const i_amqp09::longstr_t response_,
            const i_amqp09::shortstr_t locale_);

        void connection_secure (
            const i_amqp09::longstr_t challenge_);

        void connection_secure_ok (
            const i_amqp09::longstr_t response_);

        void connection_tune (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_tune_ok (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_open (
            const i_amqp09::shortstr_t virtual_host_,
            const i_amqp09::shortstr_t capabilities_,
            bool insist_);

        void connection_open_ok (
            const i_amqp09::shortstr_t known_hosts_);

        void connection_redirect (
            const i_amqp09::shortstr_t host_,
            const i_amqp09::shortstr_t known_hosts_);

        void connection_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        void connection_close_ok ();

        void channel_open (
            const i_amqp09::shortstr_t out_of_band_);

        void channel_open_ok (
            const i_amqp09::longstr_t channel_id_);

        void channel_flow (
            bool active_);

        void channel_flow_ok (
            bool active_);

        void channel_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        void channel_close_ok ();

        void channel_resume (
            const i_amqp09::longstr_t channel_id_);

        void channel_ping ();

        void channel_pong ();

        void channel_ok ();

        void access_request (
            const i_amqp09::shortstr_t realm_,
            bool exclusive_,
            bool passive_,
            bool active_,
            bool write_,
            bool read_);

        void access_request_ok (
            uint16_t ticket_);

        void exchange_declare (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t type_,
            bool passive_,
            bool durable_,
            bool auto_delete_,
            bool internal_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_);

        void exchange_declare_ok ();

        void exchange_delete (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            bool if_unused_,
            bool nowait_);

        void exchange_delete_ok ();

        void queue_declare (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_);

        void queue_declare_ok (
            const i_amqp09::shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_);

        void queue_bind (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_);

        void queue_bind_ok ();

        void queue_unbind (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            const i_amqp09::field_table_t &arguments_);

        void queue_unbind_ok ();

        void queue_purge (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool nowait_);

        void queue_purge_ok (
            uint32_t message_count_);

        void queue_delete (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool nowait_);

        void queue_delete_ok (
            uint32_t message_count_);

        void basic_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_);

        void basic_qos_ok ();

        void basic_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_);

        void basic_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_);

        void basic_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_);

        void basic_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_);

        void basic_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_);

        void basic_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_);

        void basic_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_);

        void basic_get (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool no_ack_);

        void basic_get_ok (
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            uint32_t message_count_);

        void basic_get_empty (
            const i_amqp09::shortstr_t cluster_id_);

        void basic_ack (
            uint64_t delivery_tag_,
            bool multiple_);

        void basic_reject (
            uint64_t delivery_tag_,
            bool requeue_);

        void basic_recover (
            bool requeue_);

        void file_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_);

        void file_qos_ok ();

        void file_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_);

        void file_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_);

        void file_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_);

        void file_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_);

        void file_open (
            const i_amqp09::shortstr_t identifier_,
            uint64_t content_size_);

        void file_open_ok (
            uint64_t staged_size_);

        void file_stage ();

        void file_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_,
            const i_amqp09::shortstr_t identifier_);

        void file_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_);

        void file_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            const i_amqp09::shortstr_t identifier_);

        void file_ack (
            uint64_t delivery_tag_,
            bool multiple_);

        void file_reject (
            uint64_t delivery_tag_,
            bool requeue_);

        void stream_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            uint32_t consume_rate_,
            bool global_);

        void stream_qos_ok ();

        void stream_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_);

        void stream_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_);

        void stream_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_);

        void stream_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_);

        void stream_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_);

        void stream_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_);

        void stream_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t queue_);

        void tx_select ();

        void tx_select_ok ();

        void tx_commit ();

        void tx_commit_ok ();

        void tx_rollback ();

        void tx_rollback_ok ();

        void dtx_select ();

        void dtx_select_ok ();

        void dtx_start (
            const i_amqp09::shortstr_t dtx_identifier_);

        void dtx_start_ok ();

        void tunnel_request (
            const i_amqp09::field_table_t &meta_data_);

        //  Retrieves one command from the command queue. Returns false
        //  if there are no commands to retrieve.
        bool read (command_t *command);

    private:

        void unexpected ();

        //  Helper function used to serialise AMQP field table type into
        //  its binary representation.
        void put_field_table (unsigned char *args, size_t args_size,
            size_t &offset,const i_amqp09::field_table_t &table_);

        std::queue <command_t> command_queue;
        i_signaler *signaler;
    };

}

#endif
