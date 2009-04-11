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

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <stdlib.h>

#include <zmq/amqp_marshaller.hpp>
#include <zmq/wire.hpp>

zmq::amqp_marshaller_t::amqp_marshaller_t ()
{
}

zmq::amqp_marshaller_t::~amqp_marshaller_t ()
{
    //  Deallocate bodies of the commands stored in the command queue.
    //  This won't get done automatically!
    while (!command_queue.empty ()) {
        free (command_queue.front ().args);
        command_queue.pop ();
    }
}

void zmq::amqp_marshaller_t::connection_start (
            uint16_t channel_,
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp::field_table_t &server_properties_,
            const i_amqp::longstr_t mechanisms_,
            const i_amqp::longstr_t locales_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    put_uint8 (args + offset, version_major_);
    offset += sizeof (uint8_t);
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    put_uint8 (args + offset, version_minor_);
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp::frame_min_size, offset, server_properties_);
    zmq_assert (offset + sizeof (uint32_t) + mechanisms_.size <=
        i_amqp::frame_min_size);
    put_uint32 (args + offset, mechanisms_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, mechanisms_.data, mechanisms_.size);
    offset += mechanisms_.size;
    zmq_assert (offset + sizeof (uint32_t) + locales_.size <=
        i_amqp::frame_min_size);
    put_uint32 (args + offset, locales_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, locales_.data, locales_.size);
    offset += locales_.size;

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_start_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_start_ok (
            uint16_t channel_,
            const i_amqp::field_table_t &client_properties_,
            const i_amqp::shortstr_t mechanism_,
            const i_amqp::longstr_t response_,
            const i_amqp::shortstr_t locale_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    put_field_table (args, i_amqp::frame_min_size, offset, client_properties_);
    zmq_assert (offset + sizeof (uint8_t) + mechanism_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, mechanism_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, mechanism_.data, mechanism_.size);
    offset += mechanism_.size;
    zmq_assert (offset + sizeof (uint32_t) + response_.size <=
        i_amqp::frame_min_size);
    put_uint32 (args + offset, response_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, response_.data, response_.size);
    offset += response_.size;
    zmq_assert (offset + sizeof (uint8_t) + locale_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, locale_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, locale_.data, locale_.size);
    offset += locale_.size;

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_start_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_secure (
            uint16_t channel_,
            const i_amqp::longstr_t challenge_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint32_t) + challenge_.size <=
        i_amqp::frame_min_size);
    put_uint32 (args + offset, challenge_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, challenge_.data, challenge_.size);
    offset += challenge_.size;

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_secure_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_secure_ok (
            uint16_t channel_,
            const i_amqp::longstr_t response_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint32_t) + response_.size <=
        i_amqp::frame_min_size);
    put_uint32 (args + offset, response_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, response_.data, response_.size);
    offset += response_.size;

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_secure_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_tune (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, channel_max_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, frame_max_);
    offset += sizeof (uint32_t);
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, heartbeat_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_tune_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_tune_ok (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, channel_max_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, frame_max_);
    offset += sizeof (uint32_t);
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, heartbeat_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_tune_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_open (
            uint16_t channel_,
            const i_amqp::shortstr_t virtual_host_,
            const i_amqp::shortstr_t reserved_1_,
            bool reserved_2_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + virtual_host_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, virtual_host_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, virtual_host_.data, virtual_host_.size);
    offset += virtual_host_.size;
    zmq_assert (offset + sizeof (uint8_t) + reserved_1_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, reserved_1_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reserved_1_.data, reserved_1_.size);
    offset += reserved_1_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((reserved_2_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_open_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_open_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t reserved_1_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + reserved_1_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, reserved_1_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reserved_1_.data, reserved_1_.size);
    offset += reserved_1_.size;

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_open_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + reply_text_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, class_id_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, method_id_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_close_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::connection_close_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::connection_id,
        i_amqp::connection_close_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::channel_open (
            uint16_t channel_,
            const i_amqp::shortstr_t reserved_1_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + reserved_1_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, reserved_1_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reserved_1_.data, reserved_1_.size);
    offset += reserved_1_.size;

    command_t cmd = {
        channel_,
        i_amqp::channel_id,
        i_amqp::channel_open_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::channel_open_ok (
            uint16_t channel_,
            const i_amqp::longstr_t reserved_1_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint32_t) + reserved_1_.size <=
        i_amqp::frame_min_size);
    put_uint32 (args + offset, reserved_1_.size);
    offset += sizeof (uint32_t);
    memcpy (args + offset, reserved_1_.data, reserved_1_.size);
    offset += reserved_1_.size;

    command_t cmd = {
        channel_,
        i_amqp::channel_id,
        i_amqp::channel_open_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::channel_flow (
            uint16_t channel_,
            bool active_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((active_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::channel_id,
        i_amqp::channel_flow_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::channel_flow_ok (
            uint16_t channel_,
            bool active_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((active_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::channel_id,
        i_amqp::channel_flow_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::channel_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + reply_text_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, class_id_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, method_id_);
    offset += sizeof (uint16_t);

    command_t cmd = {
        channel_,
        i_amqp::channel_id,
        i_amqp::channel_close_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::channel_close_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::channel_id,
        i_amqp::channel_close_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::exchange_declare (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t type_,
            bool passive_,
            bool durable_,
            bool reserved_2_,
            bool reserved_3_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) + type_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, type_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, type_.data, type_.size);
    offset += type_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((passive_ ? 1 : 0) << 0) |
        ((durable_ ? 1 : 0) << 1) |
        ((reserved_2_ ? 1 : 0) << 2) |
        ((reserved_3_ ? 1 : 0) << 3) |
        ((no_wait_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp::frame_min_size, offset, arguments_);

    command_t cmd = {
        channel_,
        i_amqp::exchange_id,
        i_amqp::exchange_declare_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::exchange_declare_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::exchange_id,
        i_amqp::exchange_declare_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::exchange_delete (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t exchange_,
            bool if_unused_,
            bool no_wait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((if_unused_ ? 1 : 0) << 0) |
        ((no_wait_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::exchange_id,
        i_amqp::exchange_delete_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::exchange_delete_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::exchange_id,
        i_amqp::exchange_delete_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_declare (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((passive_ ? 1 : 0) << 0) |
        ((durable_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((auto_delete_ ? 1 : 0) << 3) |
        ((no_wait_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp::frame_min_size, offset, arguments_);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_declare_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_declare_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, consumer_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_declare_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_bind (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) + routing_key_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((no_wait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp::frame_min_size, offset, arguments_);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_bind_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_bind_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_bind_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_unbind (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            const i_amqp::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) + routing_key_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    put_field_table (args, i_amqp::frame_min_size, offset, arguments_);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_unbind_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_unbind_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_unbind_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_purge (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool no_wait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((no_wait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_purge_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_purge_ok (
            uint16_t channel_,
            uint32_t message_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_purge_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_delete (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool no_wait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((if_unused_ ? 1 : 0) << 0) |
        ((if_empty_ ? 1 : 0) << 1) |
        ((no_wait_ ? 1 : 0) << 2));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_delete_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::queue_delete_ok (
            uint16_t channel_,
            uint32_t message_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        channel_,
        i_amqp::queue_id,
        i_amqp::queue_delete_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, prefetch_size_);
    offset += sizeof (uint32_t);
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, prefetch_count_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((global_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_qos_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_qos_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_qos_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_consume (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            const i_amqp::shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool no_wait_,
            const i_amqp::field_table_t &arguments_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint8_t) + consumer_tag_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((no_local_ ? 1 : 0) << 0) |
        ((no_ack_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((no_wait_ ? 1 : 0) << 3));
    offset += sizeof (uint8_t);
    put_field_table (args, i_amqp::frame_min_size, offset, arguments_);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_consume_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_consume_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + consumer_tag_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_consume_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_cancel (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_,
            bool no_wait_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + consumer_tag_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((no_wait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_cancel_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_cancel_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + consumer_tag_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_cancel_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_publish (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) + routing_key_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((mandatory_ ? 1 : 0) << 0) |
        ((immediate_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_publish_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reply_code_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + reply_text_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) + routing_key_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_return_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_deliver (
            uint16_t channel_,
            const i_amqp::shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + consumer_tag_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    zmq_assert (offset + sizeof (uint64_t) <= i_amqp::frame_min_size);
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) + routing_key_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_deliver_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_get (
            uint16_t channel_,
            uint16_t reserved_1_,
            const i_amqp::shortstr_t queue_,
            bool no_ack_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint16_t) <= i_amqp::frame_min_size);
    put_uint16 (args + offset, reserved_1_);
    offset += sizeof (uint16_t);
    zmq_assert (offset + sizeof (uint8_t) + queue_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, queue_.data, queue_.size);
    offset += queue_.size;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((no_ack_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_get_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_get_ok (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const i_amqp::shortstr_t exchange_,
            const i_amqp::shortstr_t routing_key_,
            uint32_t message_count_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint64_t) <= i_amqp::frame_min_size);
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    zmq_assert (offset + sizeof (uint8_t) + exchange_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    zmq_assert (offset + sizeof (uint8_t) + routing_key_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    zmq_assert (offset + sizeof (uint32_t) <= i_amqp::frame_min_size);
    put_uint32 (args + offset, message_count_);
    offset += sizeof (uint32_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_get_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_get_empty (
            uint16_t channel_,
            const i_amqp::shortstr_t reserved_1_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) + reserved_1_.size <=
        i_amqp::frame_min_size);
    put_uint8 (args + offset, reserved_1_.size);
    offset += sizeof (uint8_t);
    memcpy (args + offset, reserved_1_.data, reserved_1_.size);
    offset += reserved_1_.size;

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_get_empty_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint64_t) <= i_amqp::frame_min_size);
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((multiple_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_ack_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint64_t) <= i_amqp::frame_min_size);
    put_uint64 (args + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_reject_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_recover_async (
            uint16_t channel_,
            bool requeue_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_recover_async_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_recover (
            uint16_t channel_,
            bool requeue_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;
    zmq_assert (offset + sizeof (uint8_t) <= i_amqp::frame_min_size);
    args [offset] = (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_recover_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::basic_recover_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::basic_id,
        i_amqp::basic_recover_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::tx_select (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::tx_id,
        i_amqp::tx_select_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::tx_select_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::tx_id,
        i_amqp::tx_select_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::tx_commit (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::tx_id,
        i_amqp::tx_commit_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::tx_commit_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::tx_id,
        i_amqp::tx_commit_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::tx_rollback (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::tx_id,
        i_amqp::tx_rollback_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

void zmq::amqp_marshaller_t::tx_rollback_ok (
            uint16_t channel_)
{
    unsigned char *args = (unsigned char*) malloc (i_amqp::frame_min_size);
    zmq_assert (args);

    size_t offset = 0;

    command_t cmd = {
        channel_,
        i_amqp::tx_id,
        i_amqp::tx_rollback_ok_id,
        args,
        offset
    };
    command_queue.push (cmd);
}

bool zmq::amqp_marshaller_t::read (command_t *command)
{
    //  Return false if ther is no command to retrieve
    if (command_queue.empty ())
        return false;

    //  Return the first command in the queue
    *command = command_queue.front ();
    command_queue.pop ();
    return true;
}

void zmq::amqp_marshaller_t::put_field_table (
    unsigned char *args, size_t args_size, size_t &offset,
    const i_amqp::field_table_t &table_)
{
    //  Skip field table size (to be filled in later)
    zmq_assert (offset + sizeof (uint32_t) <= args_size);
    offset += sizeof (uint32_t);
    size_t table_size = 0;

    for (i_amqp::field_table_t::const_iterator table_it = table_.begin();
          table_it != table_.end(); table_it++ ) {

        //  Put field name
        zmq_assert (offset + sizeof (uint8_t) + table_it->first.size () <=
            i_amqp::frame_min_size);
        put_uint8 (args + offset, table_it->first.size ());
        offset += sizeof (uint8_t);
        memcpy (args + offset, table_it->first.c_str (),
           table_it->first.size ());
        offset += table_it->first.size ();

        //  Put field type
        zmq_assert (offset + sizeof (uint8_t) <= args_size);
        put_uint8 (args + offset, 'S');
        offset += sizeof (uint8_t);

        //  Put field value
        zmq_assert (offset + sizeof (uint32_t) + table_it->second.size () <=
            i_amqp::frame_min_size);
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

#endif
