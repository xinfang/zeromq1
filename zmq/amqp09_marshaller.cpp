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

#include "amqp09_marshaller.hpp"
#include "wire.hpp"

zmq::amqp09_marshaller_t::amqp09_marshaller_t (i_signaler *signaler_) :
    signaler (signaler_)
{
}

zmq::amqp09_marshaller_t::~amqp09_marshaller_t ()
{
    //  Deallocate bodies of the commands stored in the command queue
    while (!command_queue.empty ()) {
        free (command_queue.front ().args);
        command_queue.pop ();
    }
}

void zmq::amqp09_marshaller_t::connection_start (
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp09::field_table_t &server_properties_,
            const i_amqp09::longstr_t mechanisms_,
            const i_amqp09::longstr_t locales_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, version_major_);
    offset += sizeof (uint8_t);
    put_uint8 (args + offset, version_minor_);
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp09::frame_min_size, offset, server_properties_);
    put_uint32 (args + offset, mechanisms_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, mechanisms_.data, mechanisms_.size);
    offset += mechanisms_.size;
    put_uint32 (args + offset, locales_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, locales_.data, locales_.size);
    offset += locales_.size;

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_start_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_start_ok (
            const i_amqp09::field_table_t &client_properties_,
            const i_amqp09::shortstr_t mechanism_,
            const i_amqp09::longstr_t response_,
            const i_amqp09::shortstr_t locale_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_field_table (args, i_amqp09::frame_min_size, offset, client_properties_);
    put_uint8 (args + offset, mechanism_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, mechanism_.data, mechanism_.size);
    offset += mechanism_.size;
    put_uint32 (args + offset, response_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, response_.data, response_.size);
    offset += response_.size;
    put_uint8 (args + offset, locale_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, locale_.data, locale_.size);
    offset += locale_.size;

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_start_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_secure (
            const i_amqp09::longstr_t challenge_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, challenge_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, challenge_.data, challenge_.size);
    offset += challenge_.size;

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_secure_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_secure_ok (
            const i_amqp09::longstr_t response_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, response_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, response_.data, response_.size);
    offset += response_.size;

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_secure_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_tune (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, channel_max_);
    offset += sizeof (uint16_t);
    put_uint32 (args + offset, frame_max_);
    offset += sizeof (uint32_t);
    put_uint16 (args + offset, heartbeat_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_tune_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_tune_ok (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, channel_max_);
    offset += sizeof (uint16_t);
    put_uint32 (args + offset, frame_max_);
    offset += sizeof (uint32_t);
    put_uint16 (args + offset, heartbeat_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_tune_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_open (
            const i_amqp09::shortstr_t virtual_host_,
            const i_amqp09::shortstr_t capabilities_,
            bool insist_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, virtual_host_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, virtual_host_.data, virtual_host_.size);
    offset += virtual_host_.size;
    put_uint8 (args + offset, capabilities_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, capabilities_.data, capabilities_.size);
    offset += capabilities_.size;
    args [offset] = (
        ((insist_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_open_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_open_ok (
            const i_amqp09::shortstr_t known_hosts_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, known_hosts_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, known_hosts_.data, known_hosts_.size);
    offset += known_hosts_.size;

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_open_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_redirect (
            const i_amqp09::shortstr_t host_,
            const i_amqp09::shortstr_t known_hosts_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, host_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, host_.data, host_.size);
    offset += host_.size;
    put_uint8 (args + offset, known_hosts_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, known_hosts_.data, known_hosts_.size);
    offset += known_hosts_.size;

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_redirect_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint16 (args + offset, class_id_);
    offset += sizeof (uint16_t);
    put_uint16 (args + offset, method_id_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_close_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::connection_close_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::connection_id,
        i_amqp09::connection_close_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_open (
            const i_amqp09::shortstr_t out_of_band_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, out_of_band_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, out_of_band_.data, out_of_band_.size);
    offset += out_of_band_.size;

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_open_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_open_ok (
            const i_amqp09::longstr_t channel_id_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, channel_id_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, channel_id_.data, channel_id_.size);
    offset += channel_id_.size;

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_open_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_flow (
            bool active_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    args [offset] = (
        ((active_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_flow_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_flow_ok (
            bool active_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    args [offset] = (
        ((active_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_flow_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_close (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint16 (args + offset, class_id_);
    offset += sizeof (uint16_t);
    put_uint16 (args + offset, method_id_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_close_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_close_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_close_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_resume (
            const i_amqp09::longstr_t channel_id_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, channel_id_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, channel_id_.data, channel_id_.size);
    offset += channel_id_.size;

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_resume_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_ping ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_ping_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_pong ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_pong_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::channel_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::channel_id,
        i_amqp09::channel_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::access_request (
            const i_amqp09::shortstr_t realm_,
            bool exclusive_,
            bool passive_,
            bool active_,
            bool write_,
            bool read_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, realm_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, realm_.data, realm_.size);
    offset += realm_.size;
    args [offset] = (
        ((exclusive_ ? 1 : 0) << 0) |
        ((passive_ ? 1 : 0) << 1) |
        ((active_ ? 1 : 0) << 2) |
        ((write_ ? 1 : 0) << 3) |
        ((read_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::access_id,
        i_amqp09::access_request_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::access_request_ok (
            uint16_t ticket_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        i_amqp09::access_id,
        i_amqp09::access_request_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::exchange_declare (
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
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, type_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, type_.data, type_.size);
    offset += type_.size;
    args [offset] = (
        ((passive_ ? 1 : 0) << 0) |
        ((durable_ ? 1 : 0) << 1) |
        ((auto_delete_ ? 1 : 0) << 2) |
        ((internal_ ? 1 : 0) << 3) |
        ((nowait_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp09::frame_min_size, offset, arguments_);

    command_t cmd = {
        i_amqp09::exchange_id,
        i_amqp09::exchange_declare_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::exchange_declare_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::exchange_id,
        i_amqp09::exchange_declare_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::exchange_delete (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            bool if_unused_,
            bool nowait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    args [offset] = (
        ((if_unused_ ? 1 : 0) << 0) |
        ((nowait_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::exchange_id,
        i_amqp09::exchange_delete_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::exchange_delete_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::exchange_id,
        i_amqp09::exchange_delete_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_declare (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    args [offset] = (
        ((passive_ ? 1 : 0) << 0) |
        ((durable_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((auto_delete_ ? 1 : 0) << 3) |
        ((nowait_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp09::frame_min_size, offset, arguments_);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_declare_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_declare_ok (
            const i_amqp09::shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);
    put_uint32 (args + offset, consumer_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_declare_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_bind (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool nowait_,
            const i_amqp09::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    args [offset] = (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp09::frame_min_size, offset, arguments_);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_bind_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_bind_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_bind_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_unbind (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            const i_amqp09::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    put_field_table (args, i_amqp09::frame_min_size, offset, arguments_);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_unbind_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_unbind_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_unbind_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_purge (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool nowait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    args [offset] = (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_purge_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_purge_ok (
            uint32_t message_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_purge_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_delete (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool nowait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    args [offset] = (
        ((if_unused_ ? 1 : 0) << 0) |
        ((if_empty_ ? 1 : 0) << 1) |
        ((nowait_ ? 1 : 0) << 2));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_delete_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::queue_delete_ok (
            uint32_t message_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        i_amqp09::queue_id,
        i_amqp09::queue_delete_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, prefetch_size_);
    offset += sizeof (uint32_t);
    put_uint16 (args + offset, prefetch_count_);
    offset += sizeof (uint16_t);
    args [offset] = (
        ((global_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_qos_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_qos_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_qos_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    args [offset] = (
        ((no_local_ ? 1 : 0) << 0) |
        ((no_ack_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((nowait_ ? 1 : 0) << 3));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp09::frame_min_size, offset, filter_);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_consume_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_consume_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    args [offset] = (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_cancel_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_cancel_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    args [offset] = (
        ((mandatory_ ? 1 : 0) << 0) |
        ((immediate_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_publish_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_return_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    args [offset] = (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_deliver_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_get (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            bool no_ack_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    args [offset] = (
        ((no_ack_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_get_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_get_ok (
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            uint32_t message_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    args [offset] = (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_get_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_get_empty (
            const i_amqp09::shortstr_t cluster_id_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, cluster_id_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, cluster_id_.data, cluster_id_.size);
    offset += cluster_id_.size;

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_get_empty_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_ack (
            uint64_t delivery_tag_,
            bool multiple_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    args [offset] = (
        ((multiple_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_ack_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_reject (
            uint64_t delivery_tag_,
            bool requeue_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    args [offset] = (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_reject_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::basic_recover (
            bool requeue_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    args [offset] = (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::basic_id,
        i_amqp09::basic_recover_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, prefetch_size_);
    offset += sizeof (uint32_t);
    put_uint16 (args + offset, prefetch_count_);
    offset += sizeof (uint16_t);
    args [offset] = (
        ((global_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_qos_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_qos_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_qos_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    args [offset] = (
        ((no_local_ ? 1 : 0) << 0) |
        ((no_ack_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((nowait_ ? 1 : 0) << 3));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp09::frame_min_size, offset, filter_);

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_consume_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_consume_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    args [offset] = (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_cancel_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_cancel_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_open (
            const i_amqp09::shortstr_t identifier_,
            uint64_t content_size_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, identifier_.data, identifier_.size);
    offset += identifier_.size;
    put_uint64 (args + offset, content_size_);
    offset += sizeof (uint64_t);

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_open_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_open_ok (
            uint64_t staged_size_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint64 (args + offset, staged_size_);
    offset += sizeof (uint64_t);

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_open_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_stage ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_stage_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_,
            const i_amqp09::shortstr_t identifier_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    args [offset] = (
        ((mandatory_ ? 1 : 0) << 0) |
        ((immediate_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);
    put_uint8 (args + offset, identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, identifier_.data, identifier_.size);
    offset += identifier_.size;

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_publish_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_return_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            const i_amqp09::shortstr_t identifier_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    args [offset] = (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    put_uint8 (args + offset, identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, identifier_.data, identifier_.size);
    offset += identifier_.size;

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_deliver_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_ack (
            uint64_t delivery_tag_,
            bool multiple_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    args [offset] = (
        ((multiple_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_ack_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::file_reject (
            uint64_t delivery_tag_,
            bool requeue_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    args [offset] = (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::file_id,
        i_amqp09::file_reject_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_qos (
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            uint32_t consume_rate_,
            bool global_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint32 (args + offset, prefetch_size_);
    offset += sizeof (uint32_t);
    put_uint16 (args + offset, prefetch_count_);
    offset += sizeof (uint16_t);
    put_uint32 (args + offset, consume_rate_);
    offset += sizeof (uint32_t);
    args [offset] = (
        ((global_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_qos_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_qos_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_qos_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_consume (
            uint16_t ticket_,
            const i_amqp09::shortstr_t queue_,
            const i_amqp09::shortstr_t consumer_tag_,
            bool no_local_,
            bool exclusive_,
            bool nowait_,
            const i_amqp09::field_table_t &filter_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    args [offset] = (
        ((no_local_ ? 1 : 0) << 0) |
        ((exclusive_ ? 1 : 0) << 1) |
        ((nowait_ ? 1 : 0) << 2));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp09::frame_min_size, offset, filter_);

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_consume_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_consume_ok (
            const i_amqp09::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_consume_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_cancel (
            const i_amqp09::shortstr_t consumer_tag_,
            bool nowait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    args [offset] = (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_cancel_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_cancel_ok (
            const i_amqp09::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_cancel_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_publish (
            uint16_t ticket_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    args [offset] = (
        ((mandatory_ ? 1 : 0) << 0) |
        ((immediate_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_publish_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_return (
            uint16_t reply_code_,
            const i_amqp09::shortstr_t reply_text_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t routing_key_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_return_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::stream_deliver (
            const i_amqp09::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            const i_amqp09::shortstr_t exchange_,
            const i_amqp09::shortstr_t queue_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;

    command_t cmd = {
        i_amqp09::stream_id,
        i_amqp09::stream_deliver_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::tx_select ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::tx_id,
        i_amqp09::tx_select_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::tx_select_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::tx_id,
        i_amqp09::tx_select_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::tx_commit ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::tx_id,
        i_amqp09::tx_commit_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::tx_commit_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::tx_id,
        i_amqp09::tx_commit_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::tx_rollback ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::tx_id,
        i_amqp09::tx_rollback_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::tx_rollback_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::tx_id,
        i_amqp09::tx_rollback_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::dtx_select ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::dtx_id,
        i_amqp09::dtx_select_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::dtx_select_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::dtx_id,
        i_amqp09::dtx_select_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::dtx_start (
            const i_amqp09::shortstr_t dtx_identifier_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_uint8 (args + offset, dtx_identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, dtx_identifier_.data, dtx_identifier_.size);
    offset += dtx_identifier_.size;

    command_t cmd = {
        i_amqp09::dtx_id,
        i_amqp09::dtx_start_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::dtx_start_ok ()
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;

    command_t cmd = {
        i_amqp09::dtx_id,
        i_amqp09::dtx_start_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

void zmq::amqp09_marshaller_t::tunnel_request (
            const i_amqp09::field_table_t &meta_data_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp09::frame_min_size);
    assert (args);

    size_t offset = 0;
    put_field_table (args, i_amqp09::frame_min_size, offset, meta_data_);

    command_t cmd = {
        i_amqp09::tunnel_id,
        i_amqp09::tunnel_request_id,
        args,
        offset
    };
    command_queue.push (cmd);
    signaler->signal (0);
}

bool zmq::amqp09_marshaller_t::read (command_t *command)
{
    //  Return false if ther is no command to retrieve
    if (command_queue.empty ())
        return false;

    //  Return the first command in the queue
    *command = command_queue.front ();
    command_queue.pop ();
    return true;
}

void zmq::amqp09_marshaller_t::unexpected ()
{
    assert (0);
}

void zmq::amqp09_marshaller_t::put_field_table (
    unsigned char *args, size_t args_size, size_t &offset,
    const i_amqp09::field_table_t &table_)
{
    //  Skip field table size (to be filled in later)
    offset += sizeof (uint32_t);
    size_t table_size = 0;

    for (i_amqp09::field_table_t::const_iterator table_it = table_.begin();
          table_it != table_.end(); table_it++ ) {

        //  Put field name
        put_uint8 (args + offset, table_it->first.size ());
        offset += sizeof (uint8_t);
        memcpy (args + offset, table_it->first.c_str (),
           table_it->first.size ());
        offset += table_it->first.size ();

        //  Put field type
        put_uint8 (args + offset, 'S');
        offset += sizeof (uint8_t);

        //  Put field value
        put_uint32 (args + offset, table_it->second.size ());
        offset += sizeof (uint32_t);
        memcpy (args + offset, table_it->second.c_str (),
            table_it->second.size ());
        offset += table_it->second.size ();

        //  Adjust actual table size
        table_size += (sizeof (uint8_t) + table_it->first.size () +
            sizeof (uint8_t) + sizeof (uint32_t) +
            table_it->second.size ());
    }

    //  Fill in the table size
    put_uint32 (args + offset - table_size - sizeof (uint32_t), table_size);
}
