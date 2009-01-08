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

#include <string>

#include <zmq/platform.hpp>
#include <zmq/pollable_factory.hpp>
#include <zmq/bp_tcp_listener.hpp>
#include <zmq/bp_tcp_engine.hpp>
#include <zmq/bp_sctp_listener.hpp>
#include <zmq/bp_sctp_engine.hpp>
#include <zmq/bp_pgm_listener.hpp>

ZMQ_EXPORT zmq::i_listener *zmq::create_listener (i_thread *calling_thread_,
    i_thread *thread_, const char *arguments_,
    int handler_thread_count_, i_thread **handler_threads_,
    bool source_, i_thread *peer_thread_, i_engine *peer_engine_,
    const char *peer_name_)
{
    std::string transport_type;
    std::string transport_args;

    std::string arguments (arguments_);
    std::string::size_type pos = arguments.find ("://");

    if (pos == std::string::npos) {
        transport_type = "bp/tcp";
        transport_args = arguments;
    }
    else {
        transport_type = arguments.substr (0, pos);
        transport_args = arguments.substr (pos + 3);
    }

    if (transport_type == "bp/tcp")
        return bp_tcp_listener_t::create (calling_thread_, thread_,
            transport_args.c_str (), handler_thread_count_, handler_threads_,
            source_, peer_thread_, peer_engine_, peer_name_);

#if defined ZMQ_HAVE_SCTP
    if (transport_type == "bp/sctp")
        return bp_sctp_listener_t::create (calling_thread_, thread_,
            transport_args.c_str (), handler_thread_count_, handler_threads_,
            source_, peer_thread_, peer_engine_, peer_name_);
#endif

#if defined ZMQ_HAVE_OPENPGM
    if (transport_type == "bp/pgm")
        return bp_pgm_listener_t::create (calling_thread_, thread_,
            transport_args.c_str (), handler_thread_count_, handler_threads_,
            source_, peer_thread_, peer_engine_, peer_name_);
#endif

    //  Unknown transport type.
    assert (false);
    return NULL;
}

ZMQ_EXPORT zmq::i_pollable *zmq::create_connection (i_thread *calling_thread_,
    i_thread *thread_, const char *arguments_, const char *local_object_)
{
    std::string transport_type;
    std::string transport_args;

    std::string arguments (arguments_);
    std::string::size_type pos = arguments.find ("://");

    if (pos == std::string::npos) {
        transport_type = "bp/tcp";
        transport_args = arguments;
    }
    else {
        transport_type = arguments.substr (0, pos);
        transport_args = arguments.substr (pos + 3);
    }

    if (transport_type == "bp/tcp")
        return bp_tcp_engine_t::create (calling_thread_, thread_,
            transport_args.c_str (), local_object_);

#if defined ZMQ_HAVE_SCTP
    if (transport_type == "bp/sctp")
        return bp_sctp_engine_t::create (calling_thread_, thread_,
            transport_args.c_str (), local_object_);
#endif

    //  Unknown transport type.
    assert (false);
    return NULL;
}
