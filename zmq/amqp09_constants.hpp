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

#ifndef __ZMQ_AMQP09_CONSTANTS_HPP_INCLUDED__
#define __ZMQ_AMQP09_CONSTANTS_HPP_INCLUDED__

namespace zmq
{

    namespace amqp09
    {
        enum {
            frame_method = 1,
            frame_header = 2,
            frame_body = 3,
            frame_oob_method = 4,
            frame_oob_header = 5,
            frame_oob_body = 6,
            frame_trace = 7,
            frame_heartbeat = 8,
            frame_min_size = 4096,
            frame_end = 206,
            reply_success = 200,
            not_delivered = 310,
            content_too_large = 311,
            no_route = 312,
            no_consumers = 313,
            connection_forced = 320,
            invalid_path = 402,
            access_refused = 403,
            not_found = 404,
            resource_locked = 405,
            precondition_failed = 406,
            frame_error = 501,
            syntax_error = 502,
            command_invalid = 503,
            channel_error = 504,
            resource_error = 506,
            not_allowed = 530,
            not_implemented = 540,
            internal_error = 541,
            connection = 10,
            channel = 20,
            access = 30,
            exchange = 40,
            queue = 50,
            basic = 60,
            file = 70,
            stream = 80,
            tx = 90,
            dtx = 100,
            tunnel = 110,
            connection_start = 10,
            connection_start_ok = 11,
            connection_secure = 20,
            connection_secure_ok = 21,
            connection_tune = 30,
            connection_tune_ok = 31,
            connection_open = 40,
            connection_open_ok = 41,
            connection_redirect = 42,
            connection_close = 50,
            connection_close_ok = 51,
            channel_open = 10,
            channel_open_ok = 11,
            channel_flow = 20,
            channel_flow_ok = 21,
            channel_close = 40,
            channel_close_ok = 41,
            channel_resume = 50,
            channel_ping = 60,
            channel_pong = 70,
            channel_ok = 80,
            access_request = 10,
            access_request_ok = 11,
            exchange_declare = 10,
            exchange_declare_ok = 11,
            exchange_delete = 20,
            exchange_delete_ok = 21,
            queue_declare = 10,
            queue_declare_ok = 11,
            queue_bind = 20,
            queue_bind_ok = 21,
            queue_unbind = 50,
            queue_unbind_ok = 51,
            queue_purge = 30,
            queue_purge_ok = 31,
            queue_delete = 40,
            queue_delete_ok = 41,
            basic_qos = 10,
            basic_qos_ok = 11,
            basic_consume = 20,
            basic_consume_ok = 21,
            basic_cancel = 30,
            basic_cancel_ok = 31,
            basic_publish = 40,
            basic_return = 50,
            basic_deliver = 60,
            basic_get = 70,
            basic_get_ok = 71,
            basic_get_empty = 72,
            basic_ack = 80,
            basic_reject = 90,
            basic_recover = 100,
            file_qos = 10,
            file_qos_ok = 11,
            file_consume = 20,
            file_consume_ok = 21,
            file_cancel = 30,
            file_cancel_ok = 31,
            file_open = 40,
            file_open_ok = 41,
            file_stage = 50,
            file_publish = 60,
            file_return = 70,
            file_deliver = 80,
            file_ack = 90,
            file_reject = 100,
            stream_qos = 10,
            stream_qos_ok = 11,
            stream_consume = 20,
            stream_consume_ok = 21,
            stream_cancel = 30,
            stream_cancel_ok = 31,
            stream_publish = 40,
            stream_return = 50,
            stream_deliver = 60,
            tx_select = 10,
            tx_select_ok = 11,
            tx_commit = 20,
            tx_commit_ok = 21,
            tx_rollback = 30,
            tx_rollback_ok = 31,
            dtx_select = 10,
            dtx_select_ok = 11,
            dtx_start = 20,
            dtx_start_ok = 21,
            tunnel_request = 10,
        };
    }

}

#endif
