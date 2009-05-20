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

#ifndef __ZMQ_BP_TCP_LISTENER_HPP_INCLUDED__
#define __ZMQ_BP_TCP_LISTENER_HPP_INCLUDED__

#include <vector>

#include <zmq/stdint.hpp>
#include <zmq/i_engine.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/tcp_listener.hpp>

namespace zmq
{

    //  BP (backend protocol) listener. Listens on a specified network
    //  interface and port and creates a BP engine for every new connection.

    class bp_tcp_listener_t : public i_engine, public i_pollable
    {
        //  Allow class factory to create this engine.
        friend class engine_factory_t;

    public:

        //  i_engine implementation.
        void start (i_thread *current_thread_, i_thread *engine_thread_);
        class i_demux *get_demux ();
        class i_mux *get_mux ();
        const char *get_arguments ();
        
        //  i_pollable implementation.
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void timer_event ();
        void unregister_event ();

    private:

        //  Creates a BP listener. Handler thread array determines
        //  the threads that will serve newly-created BP engines.
        bp_tcp_listener_t (i_thread *thread_,
            const char *interface_, int handler_thread_count_,
            i_thread **handler_threads_, bool sender_,
            i_thread *peer_thread_, i_engine *peer_engine_,
            const char *peer_name_);
        ~bp_tcp_listener_t ();

        //  Determines whether the engine serves as a local source of messages
        //  (i.e. reads them from the sockets and makes them available; false)
        //  or a local destination of messages (i.e. gathers the messages and
        //  sends them to the socket; true).
        bool sender;

        //  Associated poller object.
        i_poller *poller;

        //  Thread the engine runs within.
        i_thread *thread;

        //  Determine the engine and the object (either exchange or queue)
        //  within the engine to serve as a peer to this engine.
        i_thread *peer_thread;
        i_engine *peer_engine;
        char peer_name [256];

        //  Arguments string for this listener.
        char arguments [256];

        //  Listening socket.
        tcp_listener_t listener;

        //  Handle of the underlying socket.
        handle_t handle;

        //  The thread array to manage newly-created BP engines.
        typedef std::vector <i_thread*> handler_threads_t;
        handler_threads_t handler_threads;

        //  Points to the I/O thread to use to handle next BP connection.
        //  (Handler threads are used in round-robin fashion.)
        handler_threads_t::size_type current_handler_thread;

        bp_tcp_listener_t (const bp_tcp_listener_t&);
        void operator = (const bp_tcp_listener_t&);
    };

}

#endif
