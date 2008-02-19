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

#include <assert.h>

#include "amqp09_client_fsm.hpp"
#include "amqp09_constants.hpp"

zmq::amqp09_client_fsm_t::amqp09_client_fsm_t (tcp_socket_t *socket_,
      amqp09_marshaller_t *marshaller_,
      amqp09_engine_t <amqp09_client_fsm_t> *engine_,
      const char *in_exchange_, const char *in_routing_key_) :
    marshaller (marshaller_),
    engine (engine_),
    in_exchange (in_exchange_),
    in_routing_key (in_routing_key_)
{
    unsigned char protocol_header [] = {'A', 'M', 'Q', 'P', 1, 1, 0, 9};
    socket_->blocking_write (protocol_header, sizeof (protocol_header));
    state = expect_connection_start;
}

void zmq::amqp09_client_fsm_t::connection_start (
    uint8_t version_major_,
    uint8_t version_minor_,
    const i_amqp09::field_table_t &server_properties_,
    const i_amqp09::longstr_t mechanisms_,
    const i_amqp09::longstr_t locales_)
{
    if (state != expect_connection_start) {
        unexpected ();
        return;
    }

    assert (version_major_ == 0);
    assert (version_minor_ == 9);

    unsigned char auth_data [] = {0, 'g', 'u', 'e', 's', 't',
        0, 'g', 'u', 'e', 's', 't'};
    marshaller->connection_start_ok (i_amqp09::field_table_t (),
        "PLAIN", i_amqp09::longstr_t (auth_data, sizeof (auth_data)), "en_US");
    state = expect_connection_tune;
}

void zmq::amqp09_client_fsm_t::connection_tune (
    uint16_t channel_max_,
    uint32_t frame_max_,
    uint16_t heartbeat_)
{
    //  TODO: challenge handshaking may happen at this place
    if (state != expect_connection_tune) {
        unexpected ();
        return;
    }

    marshaller->connection_tune_ok (1, amqp09::frame_min_size, 0);
    marshaller->connection_open ("/", "", true);
    state = expect_connection_open_ok;
}

void zmq::amqp09_client_fsm_t::connection_open_ok (
    const i_amqp09::shortstr_t known_hosts_)
{
    marshaller->channel_open ("");
    state = expect_channel_open_ok;
}

void zmq::amqp09_client_fsm_t::channel_open_ok (
    const i_amqp09::longstr_t channel_id_)
{
    if (state != expect_channel_open_ok) {
        unexpected ();
        return;
    }

    marshaller->queue_declare (0, "", false, false, true, true, true,
        i_amqp09::field_table_t ());
    marshaller->queue_bind (0, "", in_exchange.c_str (),
        in_routing_key.c_str (), true, i_amqp09::field_table_t ());
    marshaller->basic_consume (0, "", "", false, true, false, false, 
        i_amqp09::field_table_t ());

    state = expect_basic_consume_ok;
}

void zmq::amqp09_client_fsm_t::basic_consume_ok (
    const i_amqp09::shortstr_t consumer_tag_)
{
    if (state != expect_basic_consume_ok) {
        unexpected ();
        return;
    }
    
    state = active;
    engine->flow (true);
}

void zmq::amqp09_client_fsm_t::unexpected ()
{
    assert (0);
}
