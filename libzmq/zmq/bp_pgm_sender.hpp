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

#ifndef __ZMQ_BP_PGM_SENDER_HPP_INCLUDED__
#define __ZMQ_BP_PGM_SENDER_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <vector>
#include <zmq/stdint.hpp>
#include <zmq/export.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/bp_encoder.hpp>
#include <zmq/pgm_socket.hpp>
#include <zmq/engine_base.hpp>
#include <zmq/i_pollable.hpp>

namespace zmq
{

    class bp_pgm_sender_t :
        public engine_base_t <false, true>,
        public i_pollable
    {

        //  Allow class factory to create this engine.
        friend class engine_factory_t;

    public:

        //  i_engine interface implemtation.
        i_pollable *cast_to_pollable ();
        void get_watermarks (int64_t *hwm_, int64_t *lwm_);
        int64_t get_swap_size ();
        const char *get_arguments ();
        void receive_from (pipe_t *pipe_);
        void revive (pipe_t *pipe_);

        //  i_pollable interface implementation.
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void timer_event ();
        void unregister_event ();

    private:

        bp_pgm_sender_t (i_thread *calling_thread_, i_thread *thread_,
            const char *interface_, i_thread *peer_thread_, 
            i_engine *peer_engine_);
    
        ~bp_pgm_sender_t ();

        //  Send one APDU with first message offset information. 
        //  Note that first 2 bytes in data_ are used to store the offset_
        //  and thus user data has to start at data_ + sizeof (uint16_t).
        size_t write_one_pkt_with_offset (unsigned char *data_, size_t size_,
            uint16_t offset_);

        //  Arguments string for this listener.
        char arguments [256];

        //  If true, engine is already shutting down, waiting for 
        //  confirmations from other threads.
        bool shutting_down;

        //  Message encoder.
        bp_encoder_t encoder;

        //  Callback to the poller.
        i_poller *poller;

        //  PGM socket.
        pgm_socket_t pgm_socket;

        //  Poll handle associated with PGM socket.
        handle_t handle;

        //  Output buffer from pgm_socket.
        unsigned char *out_buffer;

        //  Output buffer size.
        size_t out_buffer_size;

        size_t write_size;
        size_t write_pos;

        //  Offset of the first mesage in data chunk taken from encoder.
        int first_message_offset;

        bp_pgm_sender_t (const bp_pgm_sender_t&);
        void operator = (const bp_pgm_sender_t&);
    };

}

#endif

#endif
