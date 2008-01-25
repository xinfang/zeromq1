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

#ifndef __ZMQ_AMQP09_SOCKET_HPP_INCLUDED__
#define __ZMQ_AMQP09_SOCKET_HPP_INCLUDED__

#include "stdint.hpp"
#include "amqp09_base.hpp"

namespace zmq
{

    class amqp09_server_socket_t : public amqp09_base_t
    {
    public: 
        amqp09_server_socket_t (const char *address, uint16_t port);
        ~amqp09_server_socket_t ();

    protected:

        virtual void connection_start_ok (
            uint16_t channel_,
            const field_table_t &client_properties_, 
            const shortstr_t mechanism_, 
            const longstr_t response_, 
            const shortstr_t locale_);

        void connection_tune_ok (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_open (
            uint16_t channel_,
            const shortstr_t virtual_host_,
            const shortstr_t capabilities_,
            bool insist_
        );

        void channel_open (
            uint16_t channel_,
            const shortstr_t out_of_band_);

        void channel_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        void connection_close (
            uint16_t channel_,
            uint16_t reply_code_,
            const shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_
        );

        void channel_close_ok (
            uint16_t channel_);

        void connection_close_ok (
            uint16_t channel_);

        void unexpected ();

        enum state_t
        {
            expect_procol_header,
            expect_connection_start_ok,
            expect_connection_tune_ok,
            expect_connection_open,
            expect_channel_open,
            expect_channel_close_ok,
            expect_connection_close,
            expect_connection_close_ok,
            active,
            finish
        };

        state_t state;
    };

}

#endif
