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
*/

#include "amqp09_server_socket.hpp"

zmq::amqp09_server_socket_t::amqp09_server_socket_t (const char *address,
      uint16_t port) :
    amqp09_base_t (true, address, port)
{
    receive_protocol_header ();

    std::map<std::string, std::string> server_properties;
    server_properties ["product"] = "0MQ";
    state = expect_connection_start_ok;
    send_connection_start (0, 0, 9, server_properties, longstr_t ("PLAIN", 5),
        longstr_t ("en_US", 5));

    while (state != finish && state != active)
        dispatch ();
}

zmq::amqp09_server_socket_t::~amqp09_server_socket_t ()
{
    if (state == finish)
        return;

    send_channel_close (0, 100, "No error", 0, 0);
    state = expect_channel_close_ok;
    while (state != finish)
        dispatch (); 
}

void zmq::amqp09_server_socket_t::connection_start_ok (
    uint16_t channel_,
    const field_table_t &client_properties_, 
    const shortstr_t mechanism_, 
    const longstr_t response_, 
    const shortstr_t locale_)
{
printf ("1\n");
    if (state == expect_connection_start_ok) {
printf ("1\n");
        send_connection_tune (0, 1, amqp_frame_min_size, 0);
        state = expect_connection_tune_ok;
        return;
    }

    unexpected ();
}

void zmq::amqp09_server_socket_t::connection_tune_ok (
    uint16_t channel_,
    uint16_t channel_max_,
    uint32_t frame_max_,
    uint16_t heartbeat_)
{
    if (state == expect_connection_tune_ok) {
        state = expect_connection_open;
        return;
    }

    unexpected ();
}

void zmq::amqp09_server_socket_t::connection_open (
    uint16_t channel_,
    const shortstr_t virtual_host_,
    const shortstr_t capabilities_,
    bool insist_)
{
    if (state == expect_connection_open) {
        send_connection_open_ok (channel_, shortstr_t ());
        state = expect_channel_open;
        return;
    }

    unexpected ();
}

void zmq::amqp09_server_socket_t::channel_open (
    uint16_t channel_,
    const shortstr_t out_of_band_)
{
    if (state == expect_channel_open) {
        send_channel_open_ok (channel_, longstr_t ("0", 1));
        state = active;
        return;
    }

    unexpected ();
}

void zmq::amqp09_server_socket_t::channel_close (
    uint16_t channel_,
    uint16_t reply_code_,
    const shortstr_t reply_text_,
    uint16_t class_id_,
    uint16_t method_id_)
{
    if (state == active) {
        send_channel_close_ok (channel_);
        state = expect_connection_close;
        return;
    }

    unexpected ();
}

void zmq::amqp09_server_socket_t::connection_close (
    uint16_t channel_,
    uint16_t reply_code_,
    const shortstr_t reply_text_,
    uint16_t class_id_,
    uint16_t method_id_)
{
    send_connection_close_ok (channel_);
    state = finish;
}

void zmq::amqp09_server_socket_t::channel_close_ok (
    uint16_t channel_)
{
    if (state == expect_channel_close_ok) {
        send_connection_close (0, 200, "No error", 0, 0);
        state = expect_connection_close_ok;
    }

    assert (0);
}

void zmq::amqp09_server_socket_t::connection_close_ok (
    uint16_t channel_)
{
    if (state == expect_connection_close_ok ) {
        state = finish;
        return;
    }

    assert (0);
}

void zmq::amqp09_server_socket_t::unexpected ()
{
    send_connection_close (0, amqp_not_implemented, "Not implemented", 0, 0);
    state = expect_connection_close_ok;
}
