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
*/

#ifndef __ZMQ_BP_PGM_ENGINE_HPP_INCLUDED__
#define __ZMQ_BP_PGM_ENGINE_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <zmq/i_pollable.hpp>
#include <zmq/bp_decoder.hpp>
#include <zmq/epgm_socket.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/export.hpp>

#include <iostream>

namespace zmq
{

    class bp_pgm_engine_t : public i_pollable
    {
    public:

        //  Creates bp_pgm_engine. Underlying PGM connection is initialised
        //  using network parameter. Local object name is simply stored
        //  and passed to error handler function when connection breaks.
        ZMQ_EXPORT static bp_pgm_engine_t *create (i_thread *calling_thread_,
            i_thread *thread_, const char *network_, 
            const char *local_object_, size_t readbuf_size_);

        //  i_engine interface implemtation
        engine_type_t type ();
        void get_watermarks (uint64_t *hwm_, uint64_t *lwm_);
        void process_command (const engine_command_t &command_);

        //  i_pollable interface implementation
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void unregister_event ();
    private:

        bp_pgm_engine_t (i_thread *calling_thread_, i_thread *thread_,
            const char *network_, const char *local_object_, 
            size_t readbuf_size_);

        ~bp_pgm_engine_t ();

        //  Callback to poller.
        i_poller *poller;

        //  demux & bp_decoder
        demux_t demux;
        bp_decoder_t decoder;
       
        //  PGM socket
        epgm_socket_t epgm_socket;

        iovec *iov;
        size_t iov_len;

        //  Poll handle associated with PGM socket
        handle_t socket_handle;

        //  Poll handle associated with engine PGM waiting pipe
        handle_t pipe_handle;
    };

}

#endif

#endif
