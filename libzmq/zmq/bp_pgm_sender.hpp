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

#ifndef __ZMQ_BP_PGM_SENDER_HPP_INCLUDED__
#define __ZMQ_BP_PGM_SENDER_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <vector>
#include <zmq/stdint.hpp>
#include <zmq/export.hpp>
#include <zmq/i_listener.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/bp_encoder.hpp>
#include <zmq/epgm_socket.hpp>

namespace zmq
{

    //  BP (backend protocol) listener. Listens on a specified network
    //  interface and port and creates a BP engine for every new connection.

    class bp_pgm_sender_t : public i_listener
    {
    public:

        //  Creates a OpenPGM listener. Handler thread array determines
        //  the threads that will serve newly-created OpenPGM engines.
        ZMQ_EXPORT static bp_pgm_sender_t *create (
            i_thread *calling_thread_, i_thread *thread_, 
            const char *interface_, bool source_, i_thread *peer_thread_, 
            i_engine *peer_engine_, const char *peer_name_);

        //  Class hierarchy 
        //  i_engine <- i_pollable <- i_listener
        //
        //  i_engine interface implemtation
        engine_type_t type ();
        void get_watermarks (uint64_t *hwm_, uint64_t *lwm_);
        void process_command (const engine_command_t &command_);

        //  i_pollable interface implementation
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void error_event ();
        void unregister_event ();

        //  i_listener interface implementation
        const char *get_arguments ();

    private:
        //  PGM listener is actually sender
        bp_pgm_sender_t (i_thread *calling_thread_, i_thread *thread_,
            const char *interface_, bool source_, i_thread *peer_thread_, 
            i_engine *peer_engine_, const char *peer_name_);
        ~bp_pgm_sender_t ();

        //  Determines whether the engine serves as a local source of messages
        //  (i.e. reads them from the sockets and makes them available) or
        //  a local destination of messages (i.e. gathers the messages and
        //  sends them to the socket).
        bool source;

        //  The thread listener is running in.
        i_thread *thread;

        //  Determine the engine and the object (either exchange or queue)
        //  within the engine to serve as a peer to this engine.
        i_thread *peer_thread;
        i_engine *peer_engine;
        char peer_name [16];

        //  Arguments string for this listener.
        char arguments [256];

        //  If true, engine is already shutting down, waiting for confirmations
        //  from other threads.
        bool shutting_down;

        //  mux & bp_encoder
        mux_t mux; 
        bp_encoder_t encoder;

        //  Callback to the poller.
        i_poller *poller;

        //  PGM socket
        epgm_socket_t epgm_socket;

        //  Poll handle associated with PGM socket
        handle_t handle;

        //  Buffer from transmit window
        unsigned char *txw_slice;

        // Taken from transmit window
        size_t max_tsdu;

        size_t write_size;
        size_t write_pos;

        ssize_t first_message_offest;

        bp_pgm_sender_t (const bp_pgm_sender_t&);
        void operator = (const bp_pgm_sender_t&);
    };

}

#endif

#endif
