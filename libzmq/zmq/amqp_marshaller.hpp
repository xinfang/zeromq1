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

#ifndef __ZMQ_AMQP_MARSHALLER_HPP_INCLUDED__
#define __ZMQ_AMQP_MARSHALLER_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <queue>

#include <zmq/i_amqp.hpp>

namespace zmq
{

    //  Marshaller class converts calls to i_amqp interface into corresponding
    //  binary representation. Binary representations are stored in a queue to
    //  be retrieved by AMQP encoder when needed.

    class amqp_marshaller_t : public i_amqp
    {
    public:

        //  Structure to hold binary representation of AMQP command.
        //  'args' member (parameters of the command in binary format) is
        //  owned by this structure, thus you have to free the buffer when
        //  destroying the command. Args should be allocated using standard
        //  malloc and freed using standard free function.
        struct command_t
        {
            uint16_t channel;
            uint16_t class_id;
            uint16_t method_id;
            unsigned char *args;
            size_t args_size;
        };

        amqp_marshaller_t ();
        ~amqp_marshaller_t ();

        void connection_start (
            uint16_t channel_,
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp::field_table_t &server_properties_,
            const i_amqp::longstr_t mechanisms_,
            const i_amqp::longstr_t locales_);

        void connection_start_ok (
            uint16_t channel_,
            const i_amqp::field_table_t &client_properties_,
            const i_amqp::shortstr_t mechanism_,
            const i_amqp::longstr_t response_,
            const i_amqp::shortstr_t locale_);

        void connection_secure (
            uint16_t channel_,
            const i_amqp::longstr_t challenge_);

        void connection_secure_ok (
            uint16_t channel_,
            const i_amqp::longstr_t response_);

        void connection_tune (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_tune_ok (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_open (
            uint16_t channel_,
            const i_amqp::shortstr_t virtual_host_,
            const i_amqp::shortstr_t reserved_1_,
            bool reserved_2_);

        void connection_open_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t reserved_1_);

        void connection_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        void connection_close_ok (
            uint16_t channel_);

        void channel_open (
            uint16_t channel_,
            const i_amqp::shortstr_t reserved_1_);

        void channel_open_ok (
            uint16_t channel_,
            const i_amqp::longstr_t reserved_1_);

        void channel_flow (
            uint16_t channel_,
            bool active_);

        void channel_flow_ok (
            uint16_t channel_,
            bool active_);

        void channel_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        void channel_close_ok (
            uint16_t channel_);

        void exchange_declare (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t type_,
            bool passive_,
            bool durable_,
            bool reserved_2_,
            bool reserved_3_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_);

        void exchange_declare_ok (
            uint16_t channel_);

        void exchange_delete (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t exchange_,
            bool if_unused_,
            bool no_wait_);

        void exchange_delete_ok (
            uint16_t channel_);

        void queue_declare (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_);

        void queue_declare_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_);

        void queue_bind (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_);

        void queue_bind_ok (
            uint16_t channel_);

        void queue_unbind (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            const i_amqp::field_table_t &arguments_);

        void queue_unbind_ok (
            uint16_t channel_);

        void queue_purge (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool no_wait_);

        void queue_purge_ok (
            uint16_t channel_,
            uint32_t message_count_);

        void queue_delete (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool no_wait_);

        void queue_delete_ok (
            uint16_t channel_,
            uint32_t message_count_);

        void basic_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_);

        void basic_qos_ok (
            uint16_t channel_);

        void basic_consume (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            const i_amqp::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_);

        void basic_consume_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_);

        void basic_cancel (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_,
            bool no_wait_);

        void basic_cancel_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_);

        void basic_publish (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_);

        void basic_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_);

        void basic_deliver (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_);

        void basic_get (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool no_ack_);

        void basic_get_ok (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            uint32_t message_count_);

        void basic_get_empty (
            uint16_t channel_,
            const i_amqp::shortstr_t reserved_1_);

        void basic_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_);

        void basic_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_);

        void basic_recover_async (
            uint16_t channel_,
            bool requeue_);

        void basic_recover (
            uint16_t channel_,
            bool requeue_);

        void basic_recover_ok (
            uint16_t channel_);

        void tx_select (
            uint16_t channel_);

        void tx_select_ok (
            uint16_t channel_);

        void tx_commit (
            uint16_t channel_);

        void tx_commit_ok (
            uint16_t channel_);

        void tx_rollback (
            uint16_t channel_);

        void tx_rollback_ok (
            uint16_t channel_);


    protected:

        //  Retrieves one command from the command queue. Returns false
        //  if there are no commands to retrieve.
        bool read (command_t *command);

    private:

        //  Helper function used to serialise AMQP field table type into
        //  its binary representation.
        void put_field_table (unsigned char *args, size_t args_size,
            size_t &offset,const i_amqp::field_table_t &table_);

        //  Queue to store marshalled commands.
        std::queue <command_t> command_queue;

        amqp_marshaller_t (const amqp_marshaller_t&);
        void operator = (const amqp_marshaller_t&);
    };

}

#endif

#endif
