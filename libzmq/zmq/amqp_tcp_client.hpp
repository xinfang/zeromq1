/*
    Copyright (c) 2007-2008 FastMQ Inc.

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
*/

#ifndef __ZMQ_AMQP_TCP_CLIENT_HPP_INCLUDED__
#define __ZMQ_AMQP_TCP_CLIENT_HPP_INCLUDED__

#if defined ZMQ_HAVE_AMQP

#include <string>

#include <zmq/export.hpp>
#include <zmq/i_amqp.hpp>
#include <zmq/i_poller.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/mux.hpp>
#include <zmq/demux.hpp>
#include <zmq/tcp_socket.hpp>
#include <zmq/amqp_encoder.hpp>
#include <zmq/amqp_decoder.hpp>

namespace zmq
{

    class amqp_tcp_client_t : public i_pollable, private i_amqp
    {
        //  Allow class factory to create this engine.
        friend class pollable_factory_t;

    public:

        //  i_pollable interface implementation.
        engine_type_t type ();
        void get_watermarks (uint64_t *hwm_, uint64_t *lwm_);
        void process_command (const engine_command_t &command_);
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void unregister_event ();

    private:

        amqp_tcp_client_t (i_thread *calling_thread_, i_thread *thread_,
            const char *hostname_, const char *local_object_,
            const char *arguments_);
        ~amqp_tcp_client_t ();

        void connection_start (
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp::field_table_t &server_properties_,
            const i_amqp::longstr_t mechanisms_,
            const i_amqp::longstr_t locales_);

        void connection_tune (
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_open_ok (
            const i_amqp::shortstr_t reserved_1_);

        void channel_open_ok (
            const i_amqp::longstr_t reserved_1_);

        void channel_close (
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);

        void connection_close (
            uint16_t reply_code_,
            const i_amqp::shortstr_t reply_text_,
            uint16_t class_id_,
            uint16_t method_id_);
        
        enum state_t
        {
            state_connecting,
            state_waiting_for_connection_start,
            state_waiting_for_connection_tune,
            state_waiting_for_connection_open_ok,
            state_waiting_for_channel_open_ok,
            state_active,
            state_shutting_down
        };

        //  State of AMQP connection.
        state_t state;

        //  Object to decode AMQP commands/messages.
        amqp_decoder_t decoder;

        //  Object to aggregate messages from inbound pipes.
        mux_t mux;

        //  Object to encode AMQP commands/messages.
        amqp_encoder_t encoder;

        //  Object to distribute messages to outbound pipes.
        demux_t demux;

        //  Buffer to be written to the underlying socket.
        unsigned char *writebuf;
        int writebuf_size;
        int write_size;
        int write_pos;

        //  Buffer to read from undrlying socket.
        unsigned char *readbuf;
        int readbuf_size;
        int read_size;
        int read_pos;

        //  AMQP/TCP socket connected to the broker.
        tcp_socket_t socket;

        //  Reference to the I/O thread managing this engine.
        i_poller *poller;

        //  Handle representing the socket in the I/O thread.
        handle_t handle;

        std::string local_object;
    };

}

#endif

#endif
