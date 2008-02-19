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
#include "i_amqp09.hpp"
#include "dispatcher_proxy.hpp"
#include "amqp09_marshaller.hpp"
#include "amqp09_engine.hpp"
#include "tcp_socket.hpp"

namespace zmq
{

    class amqp09_server_fsm_t : public i_amqp09
    {
    public:

        amqp09_server_fsm_t (tcp_socket_t *socket_,
              amqp09_marshaller_t *marshaller_,
              amqp09_engine_t <amqp09_server_fsm_t> *engine_,
              const char *in_exchange_, const char *in_routing_key_);

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

    private:

        enum state_t
        {
            expect_connection_start_ok,
            expect_connection_tune_ok,
            expect_connection_open,
            expect_channel_open,
            active,
        };

        state_t state;

        void unexpected ();

        amqp09_marshaller_t *marshaller;
        amqp09_engine_t <amqp09_server_fsm_t> *engine;
    };

}

#endif
