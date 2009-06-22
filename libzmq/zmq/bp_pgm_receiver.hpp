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

#ifndef __ZMQ_BP_PGM_RECEIVER_HPP_INCLUDED__
#define __ZMQ_BP_PGM_RECEIVER_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if (defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX) ||\
    defined ZMQ_HAVE_WINDOWS

#include <zmq/i_pollable.hpp>
#include <zmq/bp_decoder.hpp>
#include <zmq/pgm_socket.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/export.hpp>
#include <zmq/engine_base.hpp>
#include <zmq/i_pollable.hpp>

namespace zmq
{

    class bp_pgm_receiver_t :
        public engine_base_t <true, false>,
        public i_pollable
    {
    
        //  Allow class factory to create this engine.
        friend class engine_factory_t;

    public:

        //  i_engine interface implemtation.
        i_pollable *cast_to_pollable ();
        void get_watermarks (int64_t *hwm_, int64_t *lwm_);
        int64_t get_swap_size ();
        void send_to (pipe_t *pipe_);

        //  i_pollable interface implementation.
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void timer_event ();
        void unregister_event ();
        void reconnect ();

    private:

        //  Creates bp_pgm_engine. Underlying PGM connection is initialised
        //  using network_ parameter.
        bp_pgm_receiver_t (i_thread *calling_thread_, i_thread *thread_,
            const char *network_, size_t readbuf_size_, 
            const char *arguments_);

        ~bp_pgm_receiver_t ();

        //  Read exactly iov_len_ count APDUs, function returns number
        //  of bytes received. Note that if we did not join message stream 
        //  before and there is not message beginning in the APDUs being 
        //  received iov_len for such a APDUs will be 0.
#ifdef ZMQ_HAVE_WINDOWS
        int receive_with_offset (void **data_);
#else
        ssize_t receive_with_offset (void **data_);
#endif

        // If receiver joined the messages stream.
        bool joined;

        //  Callback to poller.
        i_poller *poller;

        //  If true, engine is already shutting down, waiting for 
        //  confirmations from other threads.
        bool shutting_down;

        //  Message decoder.
        bp_decoder_t decoder;
       
        //  PGM socket.
        pgm_socket_t *pgm_socket;

        //  Poll handle associated with PGM socket.
        handle_t socket_handle;
#ifdef ZMQ_HAVE_WINDOWS
        handle_t socket_listener_handle;
#else
        //  Poll handle associated with engine PGM waiting pipe.
        handle_t pipe_handle;
#endif

        bp_pgm_receiver_t (const bp_pgm_receiver_t&);
        void operator = (const bp_pgm_receiver_t&);
    };

}

#endif

#endif
