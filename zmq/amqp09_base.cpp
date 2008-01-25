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

    This file is generated from amqp0-9.xml by amqp09_base.gsl using GSL/4.
*/

#include "amqp09_base.hpp"

zmq::amqp09_base_t::amqp09_base_t (bool listen, const char *address,
       uint16_t port) :
    tcp_socket_t (listen, address, port)
{
}

void zmq::amqp09_base_t::connection_start (
            uint16_t channel_,
            uint8_t version_major_,
            uint8_t version_minor_,
            const field_table_t &server_properties_,
            const longstr_t mechanisms_,
            const longstr_t locales_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_start (
            uint16_t channel_,
            uint8_t version_major_,
            uint8_t version_minor_,
            const field_table_t &server_properties_,
            const longstr_t mechanisms_,
            const longstr_t locales_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_start);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, version_major_);
    offset += sizeof (uint8_t);
    put_uint8 (buf + offset, version_minor_);
    offset += sizeof (uint8_t);
    put_field_table (server_properties_);
    put_uint32 (buf + offset, mechanisms_.size);
    offset += sizeof (uint32_t);
    memcpy (buf + offset, mechanisms_.data, mechanisms_.size);
    offset += mechanisms_.size;
    put_uint32 (buf + offset, locales_.size);
    offset += sizeof (uint32_t);
    memcpy (buf + offset, locales_.data, locales_.size);
    offset += locales_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_start_ok (
            uint16_t channel_,
            const field_table_t &client_properties_,
            const shortstr_t mechanism_,
            const longstr_t response_,
            const shortstr_t locale_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_start_ok (
            uint16_t channel_,
            const field_table_t &client_properties_,
            const shortstr_t mechanism_,
            const longstr_t response_,
            const shortstr_t locale_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_start_ok);
    offset += sizeof (uint16_t);

    put_field_table (client_properties_);
    put_uint8 (buf + offset, mechanism_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, mechanism_.data, mechanism_.size);
    offset += mechanism_.size;
    put_uint32 (buf + offset, response_.size);
    offset += sizeof (uint32_t);
    memcpy (buf + offset, response_.data, response_.size);
    offset += response_.size;
    put_uint8 (buf + offset, locale_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, locale_.data, locale_.size);
    offset += locale_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_secure (
            uint16_t channel_,
            const longstr_t challenge_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_secure (
            uint16_t channel_,
            const longstr_t challenge_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_secure);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, challenge_.size);
    offset += sizeof (uint32_t);
    memcpy (buf + offset, challenge_.data, challenge_.size);
    offset += challenge_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_secure_ok (
            uint16_t channel_,
            const longstr_t response_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_secure_ok (
            uint16_t channel_,
            const longstr_t response_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_secure_ok);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, response_.size);
    offset += sizeof (uint32_t);
    memcpy (buf + offset, response_.data, response_.size);
    offset += response_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_tune (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_tune (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_tune);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, channel_max_);
    offset += sizeof (uint16_t);
    put_uint32 (buf + offset, frame_max_);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, heartbeat_);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_tune_ok (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_tune_ok (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_tune_ok);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, channel_max_);
    offset += sizeof (uint16_t);
    put_uint32 (buf + offset, frame_max_);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, heartbeat_);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_open (
            uint16_t channel_,
            const shortstr_t virtual_host_,
            const shortstr_t capabilities_,
            bool insist_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_open (
            uint16_t channel_,
            const shortstr_t virtual_host_,
            const shortstr_t capabilities_,
            bool insist_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_open);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, virtual_host_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, virtual_host_.data, virtual_host_.size);
    offset += virtual_host_.size;
    put_uint8 (buf + offset, capabilities_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, capabilities_.data, capabilities_.size);
    offset += capabilities_.size;
    buf [offset] |= (
        ((insist_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_open_ok (
            uint16_t channel_,
            const shortstr_t known_hosts_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_open_ok (
            uint16_t channel_,
            const shortstr_t known_hosts_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_open_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, known_hosts_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, known_hosts_.data, known_hosts_.size);
    offset += known_hosts_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_redirect (
            uint16_t channel_,
            const shortstr_t host_,
            const shortstr_t known_hosts_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_redirect (
            uint16_t channel_,
            const shortstr_t host_,
            const shortstr_t known_hosts_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_redirect);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, host_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, host_.data, host_.size);
    offset += host_.size;
    put_uint8 (buf + offset, known_hosts_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, known_hosts_.data, known_hosts_.size);
    offset += known_hosts_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_close);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint16 (buf + offset, class_id_);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, method_id_);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::connection_close_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_connection_close_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_connection);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_connection_close_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_open (
            uint16_t channel_,
            const shortstr_t out_of_band_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_open (
            uint16_t channel_,
            const shortstr_t out_of_band_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_open);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, out_of_band_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, out_of_band_.data, out_of_band_.size);
    offset += out_of_band_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_open_ok (
            uint16_t channel_,
            const longstr_t channel_id_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_open_ok (
            uint16_t channel_,
            const longstr_t channel_id_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_open_ok);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, channel_id_.size);
    offset += sizeof (uint32_t);
    memcpy (buf + offset, channel_id_.data, channel_id_.size);
    offset += channel_id_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_flow (
            uint16_t channel_,
            bool active_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_flow (
            uint16_t channel_,
            bool active_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_flow);
    offset += sizeof (uint16_t);

    buf [offset] |= (
        ((active_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_flow_ok (
            uint16_t channel_,
            bool active_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_flow_ok (
            uint16_t channel_,
            bool active_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_flow_ok);
    offset += sizeof (uint16_t);

    buf [offset] |= (
        ((active_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_close);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint16 (buf + offset, class_id_);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, method_id_);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_close_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_close_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_close_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_resume (
            uint16_t channel_,
            const longstr_t channel_id_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_resume (
            uint16_t channel_,
            const longstr_t channel_id_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_resume);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, channel_id_.size);
    offset += sizeof (uint32_t);
    memcpy (buf + offset, channel_id_.data, channel_id_.size);
    offset += channel_id_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_ping (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_ping (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_ping);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_pong (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_pong (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_pong);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::channel_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_channel_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_channel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_channel_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::access_request (
            uint16_t channel_,
            const shortstr_t realm_,
            bool exclusive_,
            bool passive_,
            bool active_,
            bool write_,
            bool read_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_access_request (
            uint16_t channel_,
            const shortstr_t realm_,
            bool exclusive_,
            bool passive_,
            bool active_,
            bool write_,
            bool read_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_access);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_access_request);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, realm_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, realm_.data, realm_.size);
    offset += realm_.size;
    buf [offset] |= (
        ((exclusive_ ? 1 : 0) << 0) |
        ((passive_ ? 1 : 0) << 1) |
        ((active_ ? 1 : 0) << 2) |
        ((write_ ? 1 : 0) << 3) |
        ((read_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::access_request_ok (
            uint16_t channel_,
            uint16_t ticket_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_access_request_ok (
            uint16_t channel_,
            uint16_t ticket_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_access);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_access_request_ok);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::exchange_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t type_,
            bool passive_,
            bool durable_,
            bool auto_delete_,
            bool internal_,
            bool nowait_,
            const field_table_t &arguments_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_exchange_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t type_,
            bool passive_,
            bool durable_,
            bool auto_delete_,
            bool internal_,
            bool nowait_,
            const field_table_t &arguments_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_exchange);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_exchange_declare);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, type_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, type_.data, type_.size);
    offset += type_.size;
    buf [offset] |= (
        ((passive_ ? 1 : 0) << 0) |
        ((durable_ ? 1 : 0) << 1) |
        ((auto_delete_ ? 1 : 0) << 2) |
        ((internal_ ? 1 : 0) << 3) |
        ((nowait_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);
    put_field_table (arguments_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::exchange_declare_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_exchange_declare_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_exchange);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_exchange_declare_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::exchange_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            bool if_unused_,
            bool nowait_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_exchange_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            bool if_unused_,
            bool nowait_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_exchange);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_exchange_delete);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    buf [offset] |= (
        ((if_unused_ ? 1 : 0) << 0) |
        ((nowait_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::exchange_delete_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_exchange_delete_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_exchange);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_exchange_delete_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const field_table_t &arguments_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_declare (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool passive_,
            bool durable_,
            bool exclusive_,
            bool auto_delete_,
            bool nowait_,
            const field_table_t &arguments_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_declare);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    buf [offset] |= (
        ((passive_ ? 1 : 0) << 0) |
        ((durable_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((auto_delete_ ? 1 : 0) << 3) |
        ((nowait_ ? 1 : 0) << 4));
    offset += sizeof (uint8_t);
    put_field_table (arguments_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_declare_ok (
            uint16_t channel_,
            const shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_declare_ok (
            uint16_t channel_,
            const shortstr_t queue_,
            uint32_t message_count_,
            uint32_t consumer_count_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_declare_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint32 (buf + offset, message_count_);
    offset += sizeof (uint32_t);
    put_uint32 (buf + offset, consumer_count_);
    offset += sizeof (uint32_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_bind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool nowait_,
            const field_table_t &arguments_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_bind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool nowait_,
            const field_table_t &arguments_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_bind);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    buf [offset] |= (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_field_table (arguments_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_bind_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_bind_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_bind_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_unbind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const field_table_t &arguments_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_unbind (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const field_table_t &arguments_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_unbind);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    put_field_table (arguments_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_unbind_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_unbind_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_unbind_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_purge (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool nowait_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_purge (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool nowait_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_purge);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    buf [offset] |= (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_purge_ok (
            uint16_t channel_,
            uint32_t message_count_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_purge_ok (
            uint16_t channel_,
            uint32_t message_count_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_purge_ok);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, message_count_);
    offset += sizeof (uint32_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool nowait_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_delete (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool if_unused_,
            bool if_empty_,
            bool nowait_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_delete);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    buf [offset] |= (
        ((if_unused_ ? 1 : 0) << 0) |
        ((if_empty_ ? 1 : 0) << 1) |
        ((nowait_ ? 1 : 0) << 2));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::queue_delete_ok (
            uint16_t channel_,
            uint32_t message_count_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_queue_delete_ok (
            uint16_t channel_,
            uint32_t message_count_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_queue);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_queue_delete_ok);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, message_count_);
    offset += sizeof (uint32_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_qos);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, prefetch_size_);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, prefetch_count_);
    offset += sizeof (uint16_t);
    buf [offset] |= (
        ((global_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_qos_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_qos_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_qos_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_consume);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    buf [offset] |= (
        ((no_local_ ? 1 : 0) << 0) |
        ((no_ack_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((nowait_ ? 1 : 0) << 3));
    offset += sizeof (uint8_t);
    put_field_table (filter_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_consume_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_cancel);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    buf [offset] |= (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_cancel_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_publish);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    buf [offset] |= (
        ((mandatory_ ? 1 : 0) << 0) |
        ((immediate_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_return);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_deliver);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    buf [offset] |= (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_get (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool no_ack_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_get (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            bool no_ack_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_get);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    buf [offset] |= (
        ((no_ack_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_get_ok (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            uint32_t message_count_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_get_ok (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            uint32_t message_count_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_get_ok);
    offset += sizeof (uint16_t);

    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    buf [offset] |= (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    put_uint32 (buf + offset, message_count_);
    offset += sizeof (uint32_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_get_empty (
            uint16_t channel_,
            const shortstr_t cluster_id_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_get_empty (
            uint16_t channel_,
            const shortstr_t cluster_id_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_get_empty);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, cluster_id_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, cluster_id_.data, cluster_id_.size);
    offset += cluster_id_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_ack);
    offset += sizeof (uint16_t);

    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    buf [offset] |= (
        ((multiple_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_reject);
    offset += sizeof (uint16_t);

    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    buf [offset] |= (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::basic_recover (
            uint16_t channel_,
            bool requeue_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_basic_recover (
            uint16_t channel_,
            bool requeue_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_basic);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_basic_recover);
    offset += sizeof (uint16_t);

    buf [offset] |= (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            bool global_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_qos);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, prefetch_size_);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, prefetch_count_);
    offset += sizeof (uint16_t);
    buf [offset] |= (
        ((global_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_qos_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_qos_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_qos_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool no_ack_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_consume);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    buf [offset] |= (
        ((no_local_ ? 1 : 0) << 0) |
        ((no_ack_ ? 1 : 0) << 1) |
        ((exclusive_ ? 1 : 0) << 2) |
        ((nowait_ ? 1 : 0) << 3));
    offset += sizeof (uint8_t);
    put_field_table (filter_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_consume_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_cancel);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    buf [offset] |= (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_cancel_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_open (
            uint16_t channel_,
            const shortstr_t identifier_,
            uint64_t content_size_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_open (
            uint16_t channel_,
            const shortstr_t identifier_,
            uint64_t content_size_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_open);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, identifier_.data, identifier_.size);
    offset += identifier_.size;
    put_uint64 (buf + offset, content_size_);
    offset += sizeof (uint64_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_open_ok (
            uint16_t channel_,
            uint64_t staged_size_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_open_ok (
            uint16_t channel_,
            uint64_t staged_size_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_open_ok);
    offset += sizeof (uint16_t);

    put_uint64 (buf + offset, staged_size_);
    offset += sizeof (uint64_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_stage (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_stage (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_stage);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_,
            const shortstr_t identifier_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_,
            const shortstr_t identifier_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_publish);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    buf [offset] |= (
        ((mandatory_ ? 1 : 0) << 0) |
        ((immediate_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);
    put_uint8 (buf + offset, identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, identifier_.data, identifier_.size);
    offset += identifier_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_return);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const shortstr_t identifier_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            bool redelivered_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            const shortstr_t identifier_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_deliver);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    buf [offset] |= (
        ((redelivered_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    put_uint8 (buf + offset, identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, identifier_.data, identifier_.size);
    offset += identifier_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_ack (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool multiple_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_ack);
    offset += sizeof (uint16_t);

    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    buf [offset] |= (
        ((multiple_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::file_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_file_reject (
            uint16_t channel_,
            uint64_t delivery_tag_,
            bool requeue_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_file);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_file_reject);
    offset += sizeof (uint16_t);

    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    buf [offset] |= (
        ((requeue_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            uint32_t consume_rate_,
            bool global_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_qos (
            uint16_t channel_,
            uint32_t prefetch_size_,
            uint16_t prefetch_count_,
            uint32_t consume_rate_,
            bool global_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_qos);
    offset += sizeof (uint16_t);

    put_uint32 (buf + offset, prefetch_size_);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, prefetch_count_);
    offset += sizeof (uint16_t);
    put_uint32 (buf + offset, consume_rate_);
    offset += sizeof (uint32_t);
    buf [offset] |= (
        ((global_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_qos_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_qos_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_qos_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_consume (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t queue_,
            const shortstr_t consumer_tag_,
            bool no_local_,
            bool exclusive_,
            bool nowait_,
            const field_table_t &filter_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_consume);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;
    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    buf [offset] |= (
        ((no_local_ ? 1 : 0) << 0) |
        ((exclusive_ ? 1 : 0) << 1) |
        ((nowait_ ? 1 : 0) << 2));
    offset += sizeof (uint8_t);
    put_field_table (filter_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_consume_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_consume_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_cancel (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            bool nowait_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_cancel);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    buf [offset] |= (
        ((nowait_ ? 1 : 0) << 0));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_cancel_ok (
            uint16_t channel_,
            const shortstr_t consumer_tag_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_cancel_ok);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_publish (
            uint16_t channel_,
            uint16_t ticket_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_,
            bool mandatory_,
            bool immediate_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_publish);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, ticket_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;
    buf [offset] |= (
        ((mandatory_ ? 1 : 0) << 0) |
        ((immediate_ ? 1 : 0) << 1));
    offset += sizeof (uint8_t);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_return (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            const shortstr_t exchange_,
            const shortstr_t routing_key_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_return);
    offset += sizeof (uint16_t);

    put_uint16 (buf + offset, reply_code_);
    offset += sizeof (uint16_t);
    put_uint8 (buf + offset, reply_text_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, reply_text_.data, reply_text_.size);
    offset += reply_text_.size;
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, routing_key_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, routing_key_.data, routing_key_.size);
    offset += routing_key_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::stream_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            const shortstr_t exchange_,
            const shortstr_t queue_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_stream_deliver (
            uint16_t channel_,
            const shortstr_t consumer_tag_,
            uint64_t delivery_tag_,
            const shortstr_t exchange_,
            const shortstr_t queue_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_stream);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_stream_deliver);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, consumer_tag_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, consumer_tag_.data, consumer_tag_.size);
    offset += consumer_tag_.size;
    put_uint64 (buf + offset, delivery_tag_);
    offset += sizeof (uint64_t);
    put_uint8 (buf + offset, exchange_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, exchange_.data, exchange_.size);
    offset += exchange_.size;
    put_uint8 (buf + offset, queue_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, queue_.data, queue_.size);
    offset += queue_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::tx_select (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_tx_select (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_tx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_tx_select);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::tx_select_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_tx_select_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_tx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_tx_select_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::tx_commit (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_tx_commit (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_tx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_tx_commit);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::tx_commit_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_tx_commit_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_tx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_tx_commit_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::tx_rollback (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_tx_rollback (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_tx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_tx_rollback);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::tx_rollback_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_tx_rollback_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_tx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_tx_rollback_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::dtx_select (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_dtx_select (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_dtx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_dtx_select);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::dtx_select_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_dtx_select_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_dtx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_dtx_select_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::dtx_start (
            uint16_t channel_,
            const shortstr_t dtx_identifier_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_dtx_start (
            uint16_t channel_,
            const shortstr_t dtx_identifier_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_dtx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_dtx_start);
    offset += sizeof (uint16_t);

    put_uint8 (buf + offset, dtx_identifier_.size);
    offset += sizeof (uint8_t);
    memcpy (buf + offset, dtx_identifier_.data, dtx_identifier_.size);
    offset += dtx_identifier_.size;

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::dtx_start_ok (
            uint16_t channel_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_dtx_start_ok (
            uint16_t channel_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_dtx);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_dtx_start_ok);
    offset += sizeof (uint16_t);


    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::tunnel_request (
            uint16_t channel_,
            const field_table_t &meta_data_)
{
    unexpected ();
}

void zmq::amqp09_base_t::send_tunnel_request (
            uint16_t channel_,
            const field_table_t &meta_data_)
{
    offset = 0;

    put_uint8 (buf + offset, amqp_frame_method);
    offset += sizeof (uint8_t);
    put_uint16 (buf + offset, channel_);
    offset += sizeof (uint16_t);
    offset += sizeof (uint32_t);
    put_uint16 (buf + offset, amqp_tunnel);
    offset += sizeof (uint16_t);
    put_uint16 (buf + offset, amqp_tunnel_request);
    offset += sizeof (uint16_t);

    put_field_table (meta_data_);

    put_uint8 (buf + offset, amqp_frame_end);
    offset += sizeof (uint8_t);
    put_uint32 (buf + sizeof (uint8_t) + sizeof (uint16_t), offset -
        (sizeof (uint8_t) + sizeof (uint16_t) +  sizeof (uint32_t) +
        sizeof (uint8_t)));
    blocking_write (buf, offset);
}

void zmq::amqp09_base_t::dispatch ()
{
    offset = 0;
    blocking_read (buf, 7);
    uint8_t frame_type = get_uint8 (buf + offset);
    offset += sizeof (uint8_t);
    uint16_t channel = get_uint16 (buf + offset);
    offset += sizeof (uint16_t);
    uint32_t frame_size = get_uint32 (buf + offset);
    assert (frame_type == amqp_frame_method);

    offset = 0;
    blocking_read (buf, frame_size + 1);
    assert (buf [frame_size] == amqp_frame_end);

    uint16_t class_id = get_uint16 (buf + offset);
    offset += sizeof (uint16_t);
    uint16_t method_id = get_uint16 (buf + offset);
    offset += sizeof (uint16_t);
printf ("%d-%d\n", (int) class_id, (int) method_id);

    switch (class_id) {
    case amqp_connection:
        switch (method_id) {
        case amqp_connection_start :
            {
                uint8_t version_major = get_uint8 (buf +offset);
                offset += sizeof (uint8_t);
                uint8_t version_minor = get_uint8 (buf +offset);
                offset += sizeof (uint8_t);
                field_table_t server_properties;
                get_field_table (server_properties);
                longstr_t mechanisms;
                mechanisms.size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                mechanisms.data = (void*) (buf + offset);
                offset += mechanisms.size;
                longstr_t locales;
                locales.size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                locales.data = (void*) (buf + offset);
                offset += locales.size;

                connection_start (
                    channel,
                    version_major,
                    version_minor,
                    server_properties,
                    mechanisms,
                    locales);

                break;
            }
        case amqp_connection_start_ok :
            {
                field_table_t client_properties;
                get_field_table (client_properties);
                shortstr_t mechanism;
                mechanism.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                mechanism.data = (char*) (buf + offset);
                offset += mechanism.size;
                longstr_t response;
                response.size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                response.data = (void*) (buf + offset);
                offset += response.size;
                shortstr_t locale;
                locale.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                locale.data = (char*) (buf + offset);
                offset += locale.size;
printf ("Here!\n");
                connection_start_ok (
                    channel,
                    client_properties,
                    mechanism,
                    response,
                    locale);

                break;
            }
        case amqp_connection_secure :
            {
                longstr_t challenge;
                challenge.size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                challenge.data = (void*) (buf + offset);
                offset += challenge.size;

                connection_secure (
                    channel,
                    challenge);

                break;
            }
        case amqp_connection_secure_ok :
            {
                longstr_t response;
                response.size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                response.data = (void*) (buf + offset);
                offset += response.size;

                connection_secure_ok (
                    channel,
                    response);

                break;
            }
        case amqp_connection_tune :
            {
                uint16_t channel_max = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                uint32_t frame_max = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                uint16_t heartbeat = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);

                connection_tune (
                    channel,
                    channel_max,
                    frame_max,
                    heartbeat);

                break;
            }
        case amqp_connection_tune_ok :
            {
                uint16_t channel_max = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                uint32_t frame_max = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                uint16_t heartbeat = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);

                connection_tune_ok (
                    channel,
                    channel_max,
                    frame_max,
                    heartbeat);

                break;
            }
        case amqp_connection_open :
            {
                shortstr_t virtual_host;
                virtual_host.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                virtual_host.data = (char*) (buf + offset);
                offset += virtual_host.size;
                shortstr_t capabilities;
                capabilities.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                capabilities.data = (char*) (buf + offset);
                offset += capabilities.size;
                bool insist = buf [offset] & (0x1 << 0);

                connection_open (
                    channel,
                    virtual_host,
                    capabilities,
                    insist);

                break;
            }
        case amqp_connection_open_ok :
            {
                offset += sizeof (uint8_t);
                shortstr_t known_hosts;
                known_hosts.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                known_hosts.data = (char*) (buf + offset);
                offset += known_hosts.size;

                connection_open_ok (
                    channel,
                    known_hosts);

                break;
            }
        case amqp_connection_redirect :
            {
                shortstr_t host;
                host.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                host.data = (char*) (buf + offset);
                offset += host.size;
                shortstr_t known_hosts;
                known_hosts.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                known_hosts.data = (char*) (buf + offset);
                offset += known_hosts.size;

                connection_redirect (
                    channel,
                    host,
                    known_hosts);

                break;
            }
        case amqp_connection_close :
            {
                uint16_t reply_code = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t reply_text;
                reply_text.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (buf + offset);
                offset += reply_text.size;
                uint16_t class_id = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                uint16_t method_id = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);

                connection_close (
                    channel,
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                break;
            }
        case amqp_connection_close_ok :
            {

                connection_close_ok (
                    channel);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_channel:
        switch (method_id) {
        case amqp_channel_open :
            {
                shortstr_t out_of_band;
                out_of_band.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                out_of_band.data = (char*) (buf + offset);
                offset += out_of_band.size;

                channel_open (
                    channel,
                    out_of_band);

                break;
            }
        case amqp_channel_open_ok :
            {
                longstr_t channel_id;
                channel_id.size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                channel_id.data = (void*) (buf + offset);
                offset += channel_id.size;

                channel_open_ok (
                    channel,
                    channel_id);

                break;
            }
        case amqp_channel_flow :
            {
                bool active = buf [offset] & (0x1 << 0);

                channel_flow (
                    channel,
                    active);

                break;
            }
        case amqp_channel_flow_ok :
            {
                bool active = buf [offset] & (0x1 << 1);

                channel_flow_ok (
                    channel,
                    active);

                break;
            }
        case amqp_channel_close :
            {
                offset += sizeof (uint8_t);
                uint16_t reply_code = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t reply_text;
                reply_text.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (buf + offset);
                offset += reply_text.size;
                uint16_t class_id = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                uint16_t method_id = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);

                channel_close (
                    channel,
                    reply_code,
                    reply_text,
                    class_id,
                    method_id);

                break;
            }
        case amqp_channel_close_ok :
            {

                channel_close_ok (
                    channel);

                break;
            }
        case amqp_channel_resume :
            {
                longstr_t channel_id;
                channel_id.size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                channel_id.data = (void*) (buf + offset);
                offset += channel_id.size;

                channel_resume (
                    channel,
                    channel_id);

                break;
            }
        case amqp_channel_ping :
            {

                channel_ping (
                    channel);

                break;
            }
        case amqp_channel_pong :
            {

                channel_pong (
                    channel);

                break;
            }
        case amqp_channel_ok :
            {

                channel_ok (
                    channel);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_access:
        switch (method_id) {
        case amqp_access_request :
            {
                shortstr_t realm;
                realm.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                realm.data = (char*) (buf + offset);
                offset += realm.size;
                bool exclusive = buf [offset] & (0x1 << 0);
                bool passive = buf [offset] & (0x1 << 1);
                bool active = buf [offset] & (0x1 << 2);
                bool write = buf [offset] & (0x1 << 3);
                bool read = buf [offset] & (0x1 << 4);

                access_request (
                    channel,
                    realm,
                    exclusive,
                    passive,
                    active,
                    write,
                    read);

                break;
            }
        case amqp_access_request_ok :
            {
                offset += sizeof (uint8_t);
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);

                access_request_ok (
                    channel,
                    ticket);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_exchange:
        switch (method_id) {
        case amqp_exchange_declare :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t type;
                type.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                type.data = (char*) (buf + offset);
                offset += type.size;
                bool passive = buf [offset] & (0x1 << 0);
                bool durable = buf [offset] & (0x1 << 1);
                bool auto_delete = buf [offset] & (0x1 << 2);
                bool internal = buf [offset] & (0x1 << 3);
                bool nowait = buf [offset] & (0x1 << 4);
                offset += sizeof (uint8_t);
                field_table_t arguments;
                get_field_table (arguments);

                exchange_declare (
                    channel,
                    ticket,
                    exchange,
                    type,
                    passive,
                    durable,
                    auto_delete,
                    internal,
                    nowait,
                    arguments);

                break;
            }
        case amqp_exchange_declare_ok :
            {

                exchange_declare_ok (
                    channel);

                break;
            }
        case amqp_exchange_delete :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                bool if_unused = buf [offset] & (0x1 << 0);
                bool nowait = buf [offset] & (0x1 << 1);

                exchange_delete (
                    channel,
                    ticket,
                    exchange,
                    if_unused,
                    nowait);

                break;
            }
        case amqp_exchange_delete_ok :
            {

                exchange_delete_ok (
                    channel);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_queue:
        switch (method_id) {
        case amqp_queue_declare :
            {
                offset += sizeof (uint8_t);
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                bool passive = buf [offset] & (0x1 << 0);
                bool durable = buf [offset] & (0x1 << 1);
                bool exclusive = buf [offset] & (0x1 << 2);
                bool auto_delete = buf [offset] & (0x1 << 3);
                bool nowait = buf [offset] & (0x1 << 4);
                offset += sizeof (uint8_t);
                field_table_t arguments;
                get_field_table (arguments);

                queue_declare (
                    channel,
                    ticket,
                    queue,
                    passive,
                    durable,
                    exclusive,
                    auto_delete,
                    nowait,
                    arguments);

                break;
            }
        case amqp_queue_declare_ok :
            {
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                uint32_t message_count = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                uint32_t consumer_count = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);

                queue_declare_ok (
                    channel,
                    queue,
                    message_count,
                    consumer_count);

                break;
            }
        case amqp_queue_bind :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;
                bool nowait = buf [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                field_table_t arguments;
                get_field_table (arguments);

                queue_bind (
                    channel,
                    ticket,
                    queue,
                    exchange,
                    routing_key,
                    nowait,
                    arguments);

                break;
            }
        case amqp_queue_bind_ok :
            {

                queue_bind_ok (
                    channel);

                break;
            }
        case amqp_queue_unbind :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;
                field_table_t arguments;
                get_field_table (arguments);

                queue_unbind (
                    channel,
                    ticket,
                    queue,
                    exchange,
                    routing_key,
                    arguments);

                break;
            }
        case amqp_queue_unbind_ok :
            {

                queue_unbind_ok (
                    channel);

                break;
            }
        case amqp_queue_purge :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                bool nowait = buf [offset] & (0x1 << 0);

                queue_purge (
                    channel,
                    ticket,
                    queue,
                    nowait);

                break;
            }
        case amqp_queue_purge_ok :
            {
                offset += sizeof (uint8_t);
                uint32_t message_count = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);

                queue_purge_ok (
                    channel,
                    message_count);

                break;
            }
        case amqp_queue_delete :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                bool if_unused = buf [offset] & (0x1 << 0);
                bool if_empty = buf [offset] & (0x1 << 1);
                bool nowait = buf [offset] & (0x1 << 2);

                queue_delete (
                    channel,
                    ticket,
                    queue,
                    if_unused,
                    if_empty,
                    nowait);

                break;
            }
        case amqp_queue_delete_ok :
            {
                offset += sizeof (uint8_t);
                uint32_t message_count = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);

                queue_delete_ok (
                    channel,
                    message_count);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_basic:
        switch (method_id) {
        case amqp_basic_qos :
            {
                uint32_t prefetch_size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                uint16_t prefetch_count = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                bool global = buf [offset] & (0x1 << 0);

                basic_qos (
                    channel,
                    prefetch_size,
                    prefetch_count,
                    global);

                break;
            }
        case amqp_basic_qos_ok :
            {

                basic_qos_ok (
                    channel);

                break;
            }
        case amqp_basic_consume :
            {
                offset += sizeof (uint8_t);
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                bool no_local = buf [offset] & (0x1 << 0);
                bool no_ack = buf [offset] & (0x1 << 1);
                bool exclusive = buf [offset] & (0x1 << 2);
                bool nowait = buf [offset] & (0x1 << 3);
                offset += sizeof (uint8_t);
                field_table_t filter;
                get_field_table (filter);

                basic_consume (
                    channel,
                    ticket,
                    queue,
                    consumer_tag,
                    no_local,
                    no_ack,
                    exclusive,
                    nowait,
                    filter);

                break;
            }
        case amqp_basic_consume_ok :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;

                basic_consume_ok (
                    channel,
                    consumer_tag);

                break;
            }
        case amqp_basic_cancel :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                bool nowait = buf [offset] & (0x1 << 0);

                basic_cancel (
                    channel,
                    consumer_tag,
                    nowait);

                break;
            }
        case amqp_basic_cancel_ok :
            {
                offset += sizeof (uint8_t);
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;

                basic_cancel_ok (
                    channel,
                    consumer_tag);

                break;
            }
        case amqp_basic_publish :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;
                bool mandatory = buf [offset] & (0x1 << 0);
                bool immediate = buf [offset] & (0x1 << 1);

                basic_publish (
                    channel,
                    ticket,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate);

                break;
            }
        case amqp_basic_return :
            {
                offset += sizeof (uint8_t);
                uint16_t reply_code = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t reply_text;
                reply_text.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (buf + offset);
                offset += reply_text.size;
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;

                basic_return (
                    channel,
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                break;
            }
        case amqp_basic_deliver :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                bool redelivered = buf [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;

                basic_deliver (
                    channel,
                    consumer_tag,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key);

                break;
            }
        case amqp_basic_get :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                bool no_ack = buf [offset] & (0x1 << 0);

                basic_get (
                    channel,
                    ticket,
                    queue,
                    no_ack);

                break;
            }
        case amqp_basic_get_ok :
            {
                offset += sizeof (uint8_t);
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                bool redelivered = buf [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;
                uint32_t message_count = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);

                basic_get_ok (
                    channel,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key,
                    message_count);

                break;
            }
        case amqp_basic_get_empty :
            {
                shortstr_t cluster_id;
                cluster_id.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                cluster_id.data = (char*) (buf + offset);
                offset += cluster_id.size;

                basic_get_empty (
                    channel,
                    cluster_id);

                break;
            }
        case amqp_basic_ack :
            {
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                bool multiple = buf [offset] & (0x1 << 0);

                basic_ack (
                    channel,
                    delivery_tag,
                    multiple);

                break;
            }
        case amqp_basic_reject :
            {
                offset += sizeof (uint8_t);
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                bool requeue = buf [offset] & (0x1 << 0);

                basic_reject (
                    channel,
                    delivery_tag,
                    requeue);

                break;
            }
        case amqp_basic_recover :
            {
                bool requeue = buf [offset] & (0x1 << 1);

                basic_recover (
                    channel,
                    requeue);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_file:
        switch (method_id) {
        case amqp_file_qos :
            {
                offset += sizeof (uint8_t);
                uint32_t prefetch_size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                uint16_t prefetch_count = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                bool global = buf [offset] & (0x1 << 0);

                file_qos (
                    channel,
                    prefetch_size,
                    prefetch_count,
                    global);

                break;
            }
        case amqp_file_qos_ok :
            {

                file_qos_ok (
                    channel);

                break;
            }
        case amqp_file_consume :
            {
                offset += sizeof (uint8_t);
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                bool no_local = buf [offset] & (0x1 << 0);
                bool no_ack = buf [offset] & (0x1 << 1);
                bool exclusive = buf [offset] & (0x1 << 2);
                bool nowait = buf [offset] & (0x1 << 3);
                offset += sizeof (uint8_t);
                field_table_t filter;
                get_field_table (filter);

                file_consume (
                    channel,
                    ticket,
                    queue,
                    consumer_tag,
                    no_local,
                    no_ack,
                    exclusive,
                    nowait,
                    filter);

                break;
            }
        case amqp_file_consume_ok :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;

                file_consume_ok (
                    channel,
                    consumer_tag);

                break;
            }
        case amqp_file_cancel :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                bool nowait = buf [offset] & (0x1 << 0);

                file_cancel (
                    channel,
                    consumer_tag,
                    nowait);

                break;
            }
        case amqp_file_cancel_ok :
            {
                offset += sizeof (uint8_t);
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;

                file_cancel_ok (
                    channel,
                    consumer_tag);

                break;
            }
        case amqp_file_open :
            {
                shortstr_t identifier;
                identifier.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                identifier.data = (char*) (buf + offset);
                offset += identifier.size;
                uint64_t content_size = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);

                file_open (
                    channel,
                    identifier,
                    content_size);

                break;
            }
        case amqp_file_open_ok :
            {
                uint64_t staged_size = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);

                file_open_ok (
                    channel,
                    staged_size);

                break;
            }
        case amqp_file_stage :
            {

                file_stage (
                    channel);

                break;
            }
        case amqp_file_publish :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;
                bool mandatory = buf [offset] & (0x1 << 0);
                bool immediate = buf [offset] & (0x1 << 1);
                offset += sizeof (uint8_t);
                shortstr_t identifier;
                identifier.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                identifier.data = (char*) (buf + offset);
                offset += identifier.size;

                file_publish (
                    channel,
                    ticket,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate,
                    identifier);

                break;
            }
        case amqp_file_return :
            {
                uint16_t reply_code = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t reply_text;
                reply_text.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (buf + offset);
                offset += reply_text.size;
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;

                file_return (
                    channel,
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                break;
            }
        case amqp_file_deliver :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                bool redelivered = buf [offset] & (0x1 << 0);
                offset += sizeof (uint8_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;
                shortstr_t identifier;
                identifier.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                identifier.data = (char*) (buf + offset);
                offset += identifier.size;

                file_deliver (
                    channel,
                    consumer_tag,
                    delivery_tag,
                    redelivered,
                    exchange,
                    routing_key,
                    identifier);

                break;
            }
        case amqp_file_ack :
            {
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                bool multiple = buf [offset] & (0x1 << 0);

                file_ack (
                    channel,
                    delivery_tag,
                    multiple);

                break;
            }
        case amqp_file_reject :
            {
                offset += sizeof (uint8_t);
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                bool requeue = buf [offset] & (0x1 << 0);

                file_reject (
                    channel,
                    delivery_tag,
                    requeue);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_stream:
        switch (method_id) {
        case amqp_stream_qos :
            {
                offset += sizeof (uint8_t);
                uint32_t prefetch_size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                uint16_t prefetch_count = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                uint32_t consume_rate = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                bool global = buf [offset] & (0x1 << 0);

                stream_qos (
                    channel,
                    prefetch_size,
                    prefetch_count,
                    consume_rate,
                    global);

                break;
            }
        case amqp_stream_qos_ok :
            {

                stream_qos_ok (
                    channel);

                break;
            }
        case amqp_stream_consume :
            {
                offset += sizeof (uint8_t);
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                bool no_local = buf [offset] & (0x1 << 0);
                bool exclusive = buf [offset] & (0x1 << 1);
                bool nowait = buf [offset] & (0x1 << 2);
                offset += sizeof (uint8_t);
                field_table_t filter;
                get_field_table (filter);

                stream_consume (
                    channel,
                    ticket,
                    queue,
                    consumer_tag,
                    no_local,
                    exclusive,
                    nowait,
                    filter);

                break;
            }
        case amqp_stream_consume_ok :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;

                stream_consume_ok (
                    channel,
                    consumer_tag);

                break;
            }
        case amqp_stream_cancel :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                bool nowait = buf [offset] & (0x1 << 0);

                stream_cancel (
                    channel,
                    consumer_tag,
                    nowait);

                break;
            }
        case amqp_stream_cancel_ok :
            {
                offset += sizeof (uint8_t);
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;

                stream_cancel_ok (
                    channel,
                    consumer_tag);

                break;
            }
        case amqp_stream_publish :
            {
                uint16_t ticket = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;
                bool mandatory = buf [offset] & (0x1 << 0);
                bool immediate = buf [offset] & (0x1 << 1);

                stream_publish (
                    channel,
                    ticket,
                    exchange,
                    routing_key,
                    mandatory,
                    immediate);

                break;
            }
        case amqp_stream_return :
            {
                offset += sizeof (uint8_t);
                uint16_t reply_code = get_uint16 (buf + offset);
                offset += sizeof (uint16_t);
                shortstr_t reply_text;
                reply_text.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                reply_text.data = (char*) (buf + offset);
                offset += reply_text.size;
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t routing_key;
                routing_key.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                routing_key.data = (char*) (buf + offset);
                offset += routing_key.size;

                stream_return (
                    channel,
                    reply_code,
                    reply_text,
                    exchange,
                    routing_key);

                break;
            }
        case amqp_stream_deliver :
            {
                shortstr_t consumer_tag;
                consumer_tag.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                consumer_tag.data = (char*) (buf + offset);
                offset += consumer_tag.size;
                uint64_t delivery_tag = get_uint64 (buf + offset);
                offset += sizeof (uint64_t);
                shortstr_t exchange;
                exchange.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                exchange.data = (char*) (buf + offset);
                offset += exchange.size;
                shortstr_t queue;
                queue.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                queue.data = (char*) (buf + offset);
                offset += queue.size;

                stream_deliver (
                    channel,
                    consumer_tag,
                    delivery_tag,
                    exchange,
                    queue);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_tx:
        switch (method_id) {
        case amqp_tx_select :
            {

                tx_select (
                    channel);

                break;
            }
        case amqp_tx_select_ok :
            {

                tx_select_ok (
                    channel);

                break;
            }
        case amqp_tx_commit :
            {

                tx_commit (
                    channel);

                break;
            }
        case amqp_tx_commit_ok :
            {

                tx_commit_ok (
                    channel);

                break;
            }
        case amqp_tx_rollback :
            {

                tx_rollback (
                    channel);

                break;
            }
        case amqp_tx_rollback_ok :
            {

                tx_rollback_ok (
                    channel);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_dtx:
        switch (method_id) {
        case amqp_dtx_select :
            {

                dtx_select (
                    channel);

                break;
            }
        case amqp_dtx_select_ok :
            {

                dtx_select_ok (
                    channel);

                break;
            }
        case amqp_dtx_start :
            {
                shortstr_t dtx_identifier;
                dtx_identifier.size = get_uint8 (buf + offset);
                offset += sizeof (uint8_t);
                dtx_identifier.data = (char*) (buf + offset);
                offset += dtx_identifier.size;

                dtx_start (
                    channel,
                    dtx_identifier);

                break;
            }
        case amqp_dtx_start_ok :
            {

                dtx_start_ok (
                    channel);

                break;
            }
        default:
            assert (0);
        }
        break;
    case amqp_tunnel:
        switch (method_id) {
        case amqp_tunnel_request :
            {
                field_table_t meta_data;
                get_field_table (meta_data);

                tunnel_request (
                    channel,
                    meta_data);

                break;
            }
        default:
            assert (0);
        }
        break;
    default:
        assert (0);
    }
}

void zmq::amqp09_base_t::receive_protocol_header ()
{
    blocking_read (buf, 8);
    if (buf [0] != 'A' || buf [1] != 'M' || buf [2] != 'Q' || buf [3] != 'P')
        assert (0);
    if (buf [4] != 1 || buf [5] != 1 || buf [6] != 0 || buf [7] != 9)
        assert (0);
}

void zmq::amqp09_base_t::send_protocol_header ()
{
    unsigned char hdr [] = {'A', 'M', 'Q', 'P', 1, 1, 0, 9};
    blocking_write (hdr, sizeof (hdr));
}

void zmq::amqp09_base_t::get_field_table (field_table_t &table_)
{
    table_.clear ();
    uint32_t table_size = get_uint32 (buf + offset);
    offset += sizeof (uint32_t);
    size_t pos = 0;
    while (pos != table_size) {
        uint8_t field_name_string_size = get_uint8 (buf + offset);
        offset += sizeof (uint8_t);
        std::string field_name ((const char*) (buf + offset),
            field_name_string_size);
        offset += field_name_string_size;
        uint8_t field_type = get_uint8 (buf + offset);
        offset += sizeof (uint8_t);
        std::string field_value;
        switch (field_type) {
        case 'S':
            {
                uint32_t field_value_string_size = get_uint32 (buf + offset);
                offset += sizeof (uint32_t);
                field_value.assign ((const char*) (buf + offset),
                    field_value_string_size);
                offset += field_value_string_size;
                pos += (sizeof (uint8_t) + field_name.size () +
                    sizeof (uint8_t) + sizeof (uint32_t) + field_value.size());
                break;
            }
        default:
            assert (0);
        }    
        table_ [field_name] = field_value;
    }
}

void zmq::amqp09_base_t::put_field_table (const field_table_t &table_)
{
    offset += sizeof (uint32_t);
    size_t table_size = 0;

    for (field_table_t::const_iterator table_it = table_.begin();
          table_it != table_.end(); table_it++ ) {

        put_uint8 (buf + offset, table_it->first.size ());
        offset += sizeof (uint8_t);
        memcpy (buf + offset, table_it->first.c_str (),
           table_it->first.size ());
        offset += table_it->first.size ();
        put_uint8 (buf + offset, 'S');
        offset += sizeof (uint8_t);
        put_uint32 (buf + offset, table_it->second.size ());
        offset += sizeof (uint32_t);
        memcpy (buf + offset, table_it->second.c_str (),
            table_it->second.size ());
        offset += table_it->second.size ();
        table_size += (sizeof (uint8_t) + table_it->first.size () +
            sizeof (uint8_t) + sizeof (uint32_t) +
            table_it->second.size ());
    }
    put_uint32 (buf + offset - table_size - sizeof (uint32_t), table_size);
}

