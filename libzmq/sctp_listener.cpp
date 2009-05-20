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

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_SCTP

#include <zmq/sctp_listener.hpp>
#include <zmq/sctp_sender.hpp>
#include <zmq/sctp_receiver.hpp>
#include <zmq/config.hpp>
#include <zmq/formatting.hpp>
#include <zmq/ip.hpp>
#include <zmq/mux.hpp>
#include <zmq/data_distributor.hpp>

zmq::sctp_listener_t::sctp_listener_t (i_thread *thread_,
      const char *interface_, int handler_thread_count_,
      i_thread **handler_threads_, bool sender_,
      i_thread *peer_thread_, i_engine *peer_engine_,
      const char *peer_name_) :
    sender (sender_),
    thread (thread_),
    peer_thread (peer_thread_),
    peer_engine (peer_engine_)
{
    //  Copy the peer name.
    zmq_strncpy (peer_name, peer_name_, sizeof (peer_name));

    //  Initialise the array of threads to handle new connections.
    zmq_assert (handler_thread_count_ > 0);
    for (int thread_nbr = 0; thread_nbr != handler_thread_count_; thread_nbr ++)
        handler_threads.push_back (handler_threads_ [thread_nbr]);
    current_handler_thread = 0;

    //  Convert the hostname into sockaddr_in structure.
    sockaddr_in ip_address;
    resolve_ip_interface (&ip_address, interface_);

    //  Create a listening socket.
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    errno_assert (s != -1);

    //  Bind the socket to the network interface and port.
    int rc = bind (s, (struct sockaddr*) &ip_address, sizeof (ip_address));
    errno_assert (rc == 0);

    //  If port number was not specified, retrieve the one assigned
    //  to the socket by the operating system.
    if (ntohs (ip_address.sin_port) == 0) {
        sockaddr_in addr;
        memset (&addr, 0, sizeof (sockaddr_in));
        socklen_t sz = sizeof (sockaddr_in);
        int rc = getsockname (s, (sockaddr*) &addr, &sz);
        errno_assert (rc == 0);
        ip_address.sin_port = addr.sin_port;
    }

    //  Fill in the interface name and port.
    //  TODO: This string should be stored in the locator rather than here.
    zmq_strncpy (arguments, "sctp://", sizeof (arguments));
    size_t isz = strlen (arguments);
    if (ip_address.sin_addr.s_addr == htonl (INADDR_ANY)) {
        rc = gethostname (arguments + isz, sizeof (arguments) - isz);
        errno_assert (rc == 0);
    }
    else {
        const char *rcp = inet_ntop (AF_INET, &ip_address.sin_addr,
            arguments + isz, sizeof (arguments) - isz);
        errno_assert (rcp);
    }
    isz = strlen (arguments);
    zmq_snprintf (arguments + isz, sizeof (arguments) - isz, ":%d",
        (int) ntohs (ip_address.sin_port));

    //  Listen for incomming connections.
    rc = listen (s, 10);
    errno_assert (rc == 0);
}

zmq::sctp_listener_t::~sctp_listener_t ()
{
    //  Cleanup the socket.
    int rc = ::close (s);
    errno_assert (rc == 0);
}

void zmq::sctp_listener_t::start (i_thread *current_thread_,
    i_thread *engine_thread_)
{
    zmq_assert (thread == engine_thread_);

    //  Register SCTP engine with the I/O thread.
    command_t command;
    command.init_register_pollable (this);
    current_thread_->send_command (engine_thread_, command);
}

zmq::i_demux *zmq::sctp_listener_t::get_demux ()
{
    assert (false);
    return NULL;
}

zmq::i_mux *zmq::sctp_listener_t::get_mux ()
{
    assert (false);
    return NULL;
}

void zmq::sctp_listener_t::register_event (i_poller *poller_)
{
    handle_t handle = poller_->add_fd (s, this);
    poller_->set_pollin (handle);
}

void zmq::sctp_listener_t::in_event ()
{
    if (!sender) {
        //  Create demux for receiver engine.
        i_demux *demux = new data_distributor_t (bp_hwm, bp_lwm);
        zmq_assert (demux);

        //  Create the engine to take care of the connection.
        //  TODO: make buffer size configurable by user
        sctp_receiver_t *engine = new sctp_receiver_t (demux, s, peer_name);
        zmq_assert (engine);
        engine->start (thread, handler_threads [current_handler_thread]);

        //  The newly created engine serves as a local source of messages
        //  I.e. it reads messages from the socket and passes them on to
        //  the peer engine.
        i_thread *source_thread = handler_threads [current_handler_thread];

        //  Create the pipe to the newly created engine.
        i_mux *mux = peer_engine->get_mux ();
        pipe_t *pipe = new pipe_t (source_thread, demux,
            peer_thread, mux, mux->get_swap_size ());
        zmq_assert (pipe);

        //  Bind new engine to the source end of the pipe.
        command_t demux_cmd;
        demux_cmd.init_attach_pipe_to_demux (demux, pipe);
        thread->send_command (source_thread, demux_cmd);

        //  Bind the peer to the destination end of the pipe.
        command_t mux_cmd;
        mux_cmd.init_attach_pipe_to_mux (mux, pipe);
        thread->send_command (peer_thread, mux_cmd);

    } else {
        //  Create mux for sender engine.
        mux_t *mux = new mux_t (bp_hwm, bp_lwm);
        zmq_assert (mux);

        //  Create the engine to take care of the connection.
        //  TODO: make buffer size configurable by user
        sctp_sender_t *engine = new sctp_sender_t (mux, s, peer_name);
        zmq_assert (engine);
        engine->start (thread, handler_threads [current_handler_thread]);

        //  The newly created engine serves as a local destination of messages
        //  I.e. it sends messages received from the peer engine to the socket.
        i_thread *destination_thread =
            handler_threads [current_handler_thread];

        //  Create the pipe to the newly created engine.
        i_demux *demux = peer_engine->get_demux ();
        pipe_t *pipe = new pipe_t (peer_thread, demux,
            destination_thread, mux, mux->get_swap_size ());
        zmq_assert (pipe);

        //  Bind new engine to the destination end of the pipe.
        command_t mux_cmd;
        mux_cmd.init_attach_pipe_to_mux (mux, pipe);
        thread->send_command (destination_thread, mux_cmd);

        //  Bind the peer to the source end of the pipe.
        command_t demux_cmd;
        demux_cmd.init_attach_pipe_to_demux (demux, pipe);
        thread->send_command (peer_thread, demux_cmd);
    }

    //  Move to the next thread to get round-robin balancing of engines.
    current_handler_thread ++;
    if (current_handler_thread == handler_threads.size ())
        current_handler_thread = 0;
}

void zmq::sctp_listener_t::out_event ()
{
    //  We will never get POLLOUT when listening for incoming connections.
    zmq_assert (false);
}

void zmq::sctp_listener_t::timer_event ()
{
    //  We are setting no timers. We shouldn't get this event.
    zmq_assert (false);
}

void zmq::sctp_listener_t::unregister_event ()
{
    //  TODO: Implement this. For now we'll do nothing here.
}

const char *zmq::sctp_listener_t::get_arguments ()
{
    return arguments;
}

#endif
