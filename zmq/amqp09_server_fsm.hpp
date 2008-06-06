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

#ifndef __ZMQ_AMQP09_SERVER_FSM_HPP_INCLUDED__
#define __ZMQ_AMQP09_SERVER_FSM_HPP_INCLUDED__

#include <assert.h>
#include <string>

#include "stdint.hpp"
#include "i_context.hpp"
#include "i_amqp09.hpp"
#include "amqp09_marshaller.hpp"
#include "amqp09_engine.hpp"
#include "tcp_socket.hpp"
#include "locator.hpp"

namespace zmq
{

    //  State machine implementation for AMQP server (broker)
    //
    //  TODO: We should have special engines/threads to handle shared
    //  objects, at the moment all the objects (queues/exchanges) are
    //  owned by the connection that created them and they are destroyed
    //  when the connection closes.

    class amqp09_server_fsm_t : public i_amqp09
    {
    public:

        //  Create the state machine over the supplied TCP socket. Interconnect
        //  it with AMQP marshaller and AMQP engine. in-exchange and
        //  in-routing-key are not used.
        amqp09_server_fsm_t (i_context *context_, tcp_socket_t *socket_,
              amqp09_marshaller_t *marshaller_,
              amqp09_engine_t <amqp09_server_fsm_t> *engine_,
              locator_t *locator_);

        inline ~amqp09_server_fsm_t ()
        {
        }

        inline bool server ()
        {
            return true;
        }

        void connection_start_ok (
            const i_amqp09::field_table_t &client_properties_,
            const i_amqp09::shortstr_t mechanism_,
            const i_amqp09::longstr_t response_,
            const i_amqp09::shortstr_t locale_);

        void connection_tune_ok (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_open (
            const i_amqp09::shortstr_t virtual_host_,
            const i_amqp09::shortstr_t capabilities_,
            bool insist_);

        void channel_open (
            const i_amqp09::shortstr_t out_of_band_);

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

        void queue_declare (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_);

        void queue_bind (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_);

        void basic_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_);

        void channel_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        void connection_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

    private:

        enum state_t
        {
            expect_connection_start_ok,
            expect_connection_tune_ok,
            expect_connection_open,
            expect_channel_open,
            active,
            expect_connection_close
        };

        i_context *context;

        state_t state;

        void unexpected ();

        amqp09_marshaller_t *marshaller;
        amqp09_engine_t <amqp09_server_fsm_t> *engine;
        locator_t *locator;

        //  Next ID to use when automatic queue name generation is required
        int queue_id;
    };

}

#endif
