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
*/

#ifndef __ZMQ_AMQP_CLIENT_HPP_INCLUDED__
#define __ZMQ_AMQP_CLIENT_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <string>

#include <zmq/export.hpp>
#include <zmq/i_amqp.hpp>
#include <zmq/i_poller.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/tcp_socket.hpp>
#include <zmq/amqp_encoder.hpp>
#include <zmq/amqp_decoder.hpp>
#include <zmq/xmlParser.hpp>

namespace zmq
{

    class amqp_client_t :
        public i_engine,
        public i_pollable,
        private i_amqp
    {
        //  Allow class factory to create this engine.
        friend class engine_factory_t;

    public:

        //  i_engine interface implementation.
        void start (i_thread *current_thread_, i_thread *engine_thread_);
        i_pollable *cast_to_pollable ();
        void get_watermarks (int64_t *hwm_, int64_t *lwm_);
        class i_demux *get_demux ();
        class i_mux *get_mux ();
        void revive ();
        void head ();
        void send_to (pipe_t *pipe_);
        void receive_from (pipe_t *pipe_);

        //  i_pollable interface implementation.
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void timer_event ();
        void unregister_event ();

    private:

        amqp_client_t (mux_t *mux_, i_demux *demux_, const char *hostname_,
            const char *local_object_, const char *arguments_);
        ~amqp_client_t ();

        void connection_start (
            uint16_t channel_,
            uint8_t version_major_,
            uint8_t version_minor_,
            const i_amqp::field_table_t &/* server_properties_*/,
            const i_amqp::longstr_t /* mechanisms_ */,
            const i_amqp::longstr_t /* locales_ */);

        void connection_tune (
            uint16_t channel_,
            uint16_t channel_max_,
            uint32_t frame_max_,
            uint16_t heartbeat_);

        void connection_open_ok (
            uint16_t channel_,
            const i_amqp::shortstr_t /* reserved_1_ */);

        void channel_open_ok (
            uint16_t channel_,
            const i_amqp::longstr_t /* reserved_1_ */);

        void channel_close (
            uint16_t channel_,
            uint16_t /* reply_code_ */,
            const i_amqp::shortstr_t reply_text_,
            uint16_t /* class_id_ */,
            uint16_t /* method_id_ */);

        void connection_close (
            uint16_t channel_,
            uint16_t /* reply_code_ */,
            const i_amqp::shortstr_t reply_text_,
            uint16_t /* class_id_ */,
            uint16_t /* method_id_ */);

        void error ();
        void reconnect ();
        
        //  i_engine interface implementation.
        const char *get_arguments ();

        enum state_t
        {
            state_connecting,
            state_waiting_for_connection_start,
            state_waiting_for_connection_tune,
            state_waiting_for_connection_open_ok,
            state_waiting_for_channel_open_ok,
            state_active,
            state_waiting_for_reconnect,
            state_shutting_down
        };
        
        //  Mux & demux.
        mux_t *mux;
        i_demux *demux;

        //  State of AMQP connection.
        state_t state;

        //  ID of the AMQP channel used to pass messages.
        uint16_t channel;

        //  Object to decode AMQP commands/messages.
        amqp_decoder_t *decoder;

        //  Object to encode AMQP commands/messages.
        amqp_encoder_t *encoder;

        //  Buffer to be written to the underlying socket.
        unsigned char *writebuf;
        int writebuf_size;
        int write_size;
        int write_pos;

        //  Buffer to read from underlying socket.
        unsigned char *readbuf;
        int readbuf_size;
        int read_size;
        int read_pos;

        //  AMQP socket connected to the broker.
        tcp_socket_t socket;

        //  Reference to the I/O thread managing this engine.
        i_poller *poller;

        //  Handle representing the socket in the I/O thread.
        handle_t handle;

        std::string local_object;

        //  Configuration of AMQP environment.
        XMLNode config;

        amqp_client_t (const amqp_client_t&);
        void operator = (const amqp_client_t&);
    };

}

#endif

#endif
