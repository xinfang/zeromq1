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


#ifndef __ZMQ_BP_ENGINE_HPP_INCLUDED__
#define __ZMQ_BP_ENGINE_HPP_INCLUDED__

#include <string>

#include "export.hpp"
#include "i_engine.hpp"
#include "i_pollable.hpp"
#include "i_thread.hpp"
#include "mux.hpp"
#include "demux.hpp"
#include "bp_encoder.hpp"
#include "bp_decoder.hpp"
#include "tcp_socket.hpp"
#include "tcp_listener.hpp"

namespace zmq
{

    //  BP engine is defined by follwowing properties:
    //
    //  1. Underlying transport is TCP.
    //  2. Wire-level protocol is 0MQ backend protocol.
    //  3. Communicates with I/O thread via file descriptors.

    class bp_engine_t : public i_pollable
    {
    public:

        //  Creates bp_engine. Underlying TCP connection is initialised using
        //  host parameter. writebuf_size and readbuf_size determine
        //  the amount of batching to use. Local object name is simply stored
        //  and passed to error handler function when connection breaks.
        ZMQ_EXPORT static bp_engine_t *create (i_thread *calling_thread_,
            i_thread *thread_, const char *hostname_, size_t writebuf_size_,
            size_t readbuf_size_, const char *local_object_);

        //  Creates bp_engine from supplied listener object.
        ZMQ_EXPORT static bp_engine_t *create (i_thread *calling_thread_,
            i_thread *thread_, tcp_listener_t &listener_,
            size_t writebuf_size_, size_t readbuf_size_,
            const char *local_object_);

        //  i_pollable interface implementation.
        engine_type_t type ();
        void process_command (const engine_command_t &command_);
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void error_event ();
        void unregister_event ();


    private:

        bp_engine_t (i_thread *calling_thread_, i_thread *thread_,
            const char *hostname_,
            size_t writebuf_size_, size_t readbuf_size_,
            const char *local_object_);
        bp_engine_t (i_thread *calling_thread_, i_thread *thread_,
            tcp_listener_t &listener_, size_t writebuf_size_,
            size_t readbuf_size_, const char *local_object_);
        ~bp_engine_t ();

        //  Object to aggregate messages from inbound pipes.
        mux_t mux;

        //  Object to distribute messages to outbound pipes.
        demux_t demux;  

        //  Buffer to be written to the underlying socket.
        unsigned char *writebuf;
        size_t writebuf_size;

        //  Buffer to read from undrlying socket.
        unsigned char *readbuf;
        size_t readbuf_size;

        //  Positions in write buffer.
        size_t write_size;
        size_t write_pos;

        //  Backend wire-level protocol encoder.
        bp_encoder_t encoder;

        //  Backend wire-level protocol decoder.
        bp_decoder_t decoder;

        //  Underlying TCP/IP socket.
        tcp_socket_t socket;

        //  Callback to poller.
        i_poller *poller;

        //  Poll handle associated with this engine.
        handle_t handle;

        //  Name of the object on this side of the connection (exchange/queue).
        std::string local_object;

        //  If true, engine is already shutting down, waiting for confirmations
        //  from other threads.
        bool shutting_down;

        bp_engine_t (const bp_engine_t&);
        void operator = (const bp_engine_t&);
    };

}

#endif
