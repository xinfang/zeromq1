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

#include <string>

#include <zmq/platform.hpp>
#include <zmq/config.hpp>
#include <zmq/engine_factory.hpp>
#include <zmq/bp_tcp_listener.hpp>
#include <zmq/bp_tcp_sender.hpp>
#include <zmq/bp_tcp_receiver.hpp>
#include <zmq/sctp_listener.hpp>
#include <zmq/sctp_engine.hpp>
#include <zmq/bp_pgm_sender.hpp>
#include <zmq/bp_pgm_receiver.hpp>
#include <zmq/amqp_client.hpp>
#include <zmq/err.hpp>

zmq::i_engine *zmq::engine_factory_t::create (
    const char *name_, bool global_, bool sender_,
    const char *location_, const char *options_,
    i_thread *calling_thread_, i_thread *engine_thread_,
    int handler_thread_count_, i_thread **handler_threads_,
    i_thread *peer_thread_, i_engine *peer_engine_)
{
    //  Decompose the string to the transport name (e.g. "zmq.tcp") and
    //  transport arguments (e.g. "eth0:5555"). Default transport protocol
    //  is zmq.tcp.
    std::string transport_type;
    std::string transport_args;

    std::string location (location_);
    std::string::size_type pos = location.find ("://");

    if (pos == std::string::npos) {
        transport_type = "zmq.tcp";
        transport_args = location;
    }
    else {
        transport_type = location.substr (0, pos);
        transport_args = location.substr (pos + 3);
    }

    i_engine *engine;
    if (transport_type == "zmq.tcp") {
        if (global_)
            engine = new bp_tcp_listener_t (calling_thread_,
                engine_thread_, transport_args.c_str (), handler_thread_count_,
                handler_threads_, sender_, peer_thread_, peer_engine_,
                name_);
        else if (sender_)
            engine = new bp_tcp_receiver_t (calling_thread_, engine_thread_,
                transport_args.c_str (), name_, options_);
        else
            engine = new bp_tcp_sender_t (calling_thread_, engine_thread_,
                transport_args.c_str (), name_, options_);

        zmq_assert (engine);
        return engine;
    }

#if defined ZMQ_HAVE_OPENPGM
    if (transport_type == "zmq.pgm") {
        assert (global_ == sender_);
        if (global_)
            engine = new bp_pgm_sender_t (calling_thread_,
                engine_thread_, transport_args.c_str (), peer_thread_,
                peer_engine_);
        else
            engine = new bp_pgm_receiver_t (calling_thread_,
                engine_thread_, transport_args.c_str (), pgm_in_batch_size,
                options_);
        zmq_assert (engine);
        return engine;
    }
#endif

#if defined ZMQ_HAVE_SCTP
    if (transport_type == "sctp") {
        if (global_)
            engine = new sctp_listener_t (calling_thread_,
                engine_thread_, transport_args.c_str (), handler_thread_count_,
                handler_threads_, !sender_, peer_thread_, peer_engine_, name_);
        else
            engine = new sctp_engine_t (calling_thread_,
                engine_thread_, transport_args.c_str (), name_, options_);
        zmq_assert (engine);
        return engine;
    }
#endif

#if defined ZMQ_HAVE_AMQP
    if (transport_type == "amqp") {
        engine = new amqp_client_t (calling_thread_, engine_thread_,
            transport_args.c_str (), name_, options_);
        zmq_assert (engine);
        return engine;
    }
#endif

    //  Unknown transport type.
    zmq_assert (false);
    return NULL;
}
