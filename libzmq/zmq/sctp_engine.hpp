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


#ifndef __ZMQ_SCTP_ENGINE_HPP_INCLUDED__
#define __ZMQ_SCTP_ENGINE_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_SCTP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <zmq/stdint.hpp>
#include <zmq/export.hpp>
#include <zmq/engine_base.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/mux.hpp>
#include <zmq/demux.hpp>
#include <zmq/message.hpp>

namespace zmq
{

    //  SCTP engine is defined by follwowing properties:
    //
    //  1. Underlying transport is SCTP.
    //  2. There's no MOM-level protocol. 0MQ messages are translated
    //     directly to SCTP messages.
    //  3. Communicates with I/O thread via file descriptors.

    class sctp_engine_t :
        public engine_base_t <true, true>,
        public i_pollable
    {
        //  Allow class factory to create this engine.
        friend class engine_factory_t;

        //  Allow SCTP listener to create the engine.
        friend class sctp_listener_t;

    public:

        //  i_engine interface implementation.
        i_pollable *cast_to_pollable ();
        void get_watermarks (int64_t *hwm_, int64_t *lwm_);
        void revive (pipe_t *pipe_);
        void head (pipe_t *pipe_, int64_t position_);
        void send_to (pipe_t *pipe_);
        void receive_from (pipe_t *pipe_);

        //  i_pollable interface implementation.
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void unregister_event ();

    private:

        sctp_engine_t (i_thread *calling_thread_, i_thread *thread_,
            const char *hostname_, const char *local_object_,
            const char * /* arguments_ */);
        sctp_engine_t (i_thread *calling_thread_, i_thread *thread_,
            int listener_, const char *local_object_);
        ~sctp_engine_t ();

        //  Underlying SCTP socket.
        int s;

        //  Callback to poller.
        i_poller *poller;

        //  Poll handle associated with this engine.
        handle_t handle;

        //  Name of the object on this side of the connection (exchange/queue).
        std::string local_object;

        //  If true, engine is already shutting down, waiting for confirmations
        //  from other threads.
        bool shutting_down;

        sctp_engine_t (const sctp_engine_t&);
        void operator = (const sctp_engine_t&);
    };

}

#endif

#endif
