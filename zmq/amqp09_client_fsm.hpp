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

#ifndef __ZMQ_AMQP09_CLIENT_FSM_HPP_INCLUDED__
#define __ZMQ_AMQP09_CLIENT_FSM_HPP_INCLUDED__

#include <assert.h>

#include "stdint.hpp"
#include "i_amqp09.hpp"
#include "dispatcher_proxy.hpp"
#include "amqp09_marshaller.hpp"
#include "amqp09_engine.hpp"
#include "tcp_socket.hpp"

namespace zmq
{

    class amqp09_client_fsm_t : public i_amqp09
    {
    public:

        amqp09_client_fsm_t (tcp_socket_t *socket_,
              amqp09_marshaller_t *marshaller_,
              amqp09_engine_t <amqp09_client_fsm_t> *engine_);

        inline ~amqp09_client_fsm_t ()
        {
        }

        void connection_start (
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp09::field_table_t &server_properties_,
            const i_amqp09::longstr_t mechanisms_,
            const i_amqp09::longstr_t locales_);

        void connection_tune (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_open_ok (
            const i_amqp09::shortstr_t known_hosts_);

        void channel_open_ok (
            const i_amqp09::longstr_t channel_id_);

    private:

        enum state_t
        {
            expect_connection_start,
            expect_connection_tune,
            expect_connection_open_ok,
            expect_channel_open_ok,
            active,
        };

        state_t state;

        void unexpected ();

        amqp09_marshaller_t *marshaller;
        amqp09_engine_t <amqp09_client_fsm_t> *engine;
    };

}

#endif
