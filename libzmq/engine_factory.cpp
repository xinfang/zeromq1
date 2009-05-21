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
#include <zmq/sctp_sender.hpp>
#include <zmq/sctp_receiver.hpp>
#include <zmq/bp_pgm_sender.hpp>
#include <zmq/bp_pgm_receiver.hpp>
#include <zmq/amqp_client.hpp>
#include <zmq/data_distributor.hpp>
#include <zmq/mux.hpp>
#include <zmq/err.hpp>

zmq::i_engine *zmq::engine_factory_t::create_listener (
    const char *name_, bool sender_, const char *location_,
    i_thread *calling_thread_, i_thread *engine_thread_,
    int handler_thread_count_, i_thread **handler_threads_,
    i_thread *peer_thread_, i_engine *peer_engine_)
{
    //  Decompose the string to the transport name (e.g. "zmq.tcp") and
    //  transport arguments (e.g. "eth0:5555"). Default transport protocol
    //  is zmq.tcp.
    std::string location (location_);
    std::string::size_type pos = location.find ("://");

    std::string transport_type;
    std::string transport_args;

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
        engine = new bp_tcp_listener_t (engine_thread_,
            transport_args.c_str (), handler_thread_count_,
            handler_threads_, sender_, peer_thread_, peer_engine_, name_);
    }

#if defined ZMQ_HAVE_SCTP
    else if (transport_type == "sctp") {
        engine = new sctp_listener_t (engine_thread_,
            transport_args.c_str (), handler_thread_count_,
            handler_threads_, sender_, peer_thread_, peer_engine_, name_);
    }
#endif

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX
    else if (transport_type == "zmq.pgm") {
        zmq_assert (sender_);
        mux_t *mux = new mux_t (bp_hwm, bp_lwm);
        engine = new bp_pgm_sender_t (mux, calling_thread_,
            engine_thread_, transport_args.c_str (), peer_thread_,
            peer_engine_);
    }
#endif

    else
        //  Unknown transport type.
        zmq_assert (false);

    zmq_assert (engine);
    engine->start (calling_thread_, engine_thread_);
    return engine;
}

zmq::i_engine *zmq::engine_factory_t::create (
    const char *name_, bool sender_,
    const char *location_, const char *options_,
    i_thread *calling_thread_, i_thread *engine_thread_)
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
        if (sender_) {
            //  Create mux for sender engine.
            mux_t *mux = new mux_t (bp_hwm, bp_lwm);
            engine = new bp_tcp_sender_t (mux,
                transport_args.c_str (), name_, options_);
        }
        else {
            //  Create demux for receiver engine.
            i_demux *demux = new data_distributor_t (bp_hwm, bp_lwm);
            engine = new bp_tcp_receiver_t (demux,
                transport_args.c_str (), name_, options_);
        }
    }

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX
    else if (transport_type == "zmq.pgm") {
        if (sender_)
            zmq_assert (false);
        else {
            i_demux *demux = new data_distributor_t (bp_hwm, bp_lwm);
            engine = new bp_pgm_receiver_t (demux,
                transport_args.c_str (), pgm_in_batch_size,
                options_);
        }
    }
#endif

#if defined ZMQ_HAVE_SCTP
    else if (transport_type == "sctp") {
        if (sender_) {
            mux_t *mux = new mux_t (bp_hwm, bp_lwm);
            engine = new sctp_sender_t (mux,
                transport_args.c_str (), name_, options_);
        }
        else {
            i_demux *demux = new data_distributor_t (bp_hwm, bp_lwm);
            engine = new sctp_receiver_t (demux,
                transport_args.c_str (), name_, options_);
        }
    }
#endif

#if defined ZMQ_HAVE_AMQP
    else if (transport_type == "amqp") {
        mux_t *mux = new mux_t (bp_hwm, bp_lwm);
        i_demux *demux = new data_distributor_t (bp_hwm, bp_lwm);
        engine = new amqp_client_t (mux, demux,
            transport_args.c_str (), name_, options_);
    }
#endif

    else
        //  Unknown transport type.
        zmq_assert (false);

    zmq_assert (engine);
    engine->start (calling_thread_, engine_thread_);
    return engine;
}
