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


#ifndef __ZMQ_SCTP_SENDER_HPP_INCLUDED__
#define __ZMQ_SCTP_SENDER_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_SCTP

#include <zmq/mux.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/i_engine.hpp>
#include <zmq/i_pollable.hpp>

namespace zmq
{

    //  SCTP engine is defined by follwowing properties:
    //
    //  1. Underlying transport is SCTP.
    //  2. There's no MOM-level protocol. 0MQ messages are translated
    //     directly to SCTP messages.
    //  3. Communicates with I/O thread via file descriptors.

    class sctp_sender_t : public i_engine, public i_pollable
    {
        //  Allow class factory to create this engine.
        friend class engine_factory_t;

        //  Allow SCTP listener to create the engine.
        friend class sctp_listener_t;

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

    protected:
        const char *get_arguments ();

    private:

        sctp_sender_t (mux_t *mux_, const char *hostname_,
            const char *local_object_, const char * /* arguments_ */);
        sctp_sender_t (mux_t *mux_,
            int listener_, const char *local_object_);
        
        ~sctp_sender_t ();

        //  Mux * demux.
        mux_t *mux;

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

        sctp_sender_t (const sctp_sender_t&);
        void operator = (const sctp_sender_t&);
    };
}
#endif
#endif
