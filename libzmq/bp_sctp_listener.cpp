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

#include <zmq/platform.hpp>

#if 0 && (defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_SOLARIS)

#include <zmq/bp_sctp_listener.hpp>
#include <zmq/bp_sctp_engine.hpp>
#include <zmq/config.hpp>
#include <zmq/formatting.hpp>
#include <zmq/ip.hpp>

zmq::bp_sctp_listener_t *zmq::bp_sctp_listener_t::create (
    i_thread *calling_thread_, i_thread *thread_, const char *interface_,
    int handler_thread_count_, i_thread **handler_threads_, bool source_,
    i_thread *peer_thread_, i_engine *peer_engine_,
    const char *peer_name_)
{
    bp_sctp_listener_t *instance = new bp_sctp_listener_t (calling_thread_,
        thread_, interface_, handler_thread_count_, handler_threads_, source_,
        peer_thread_, peer_engine_, peer_name_);
    assert (instance);

    return instance;
}

zmq::bp_sctp_listener_t::bp_sctp_listener_t (i_thread *calling_thread_,
      i_thread *thread_, const char *interface_, int handler_thread_count_,
      i_thread **handler_threads_, bool source_,
      i_thread *peer_thread_, i_engine *peer_engine_,
      const char *peer_name_) :
    source (source_),
    thread (thread_),
    peer_thread (peer_thread_),
    peer_engine (peer_engine_)
{
    //  Copy the peer name.
    zmq_strncpy (peer_name, peer_name_, sizeof (peer_name));

    //  Initialise the array of threads to handle new connections.
    assert (handler_thread_count_ > 0);
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
    const char *rcp;
    size_t isz;
    zmq_strncpy (arguments, "bp/sctp://", sizeof (arguments));
    isz = strlen (arguments); 
    if (ip_address.sin_addr.s_addr == htonl (INADDR_ANY)) {
        rc = gethostname (arguments + isz, sizeof (arguments) - isz);
        assert (rc == 0);
    }
    else {
        rcp = inet_ntop (AF_INET, &ip_address.sin_addr, arguments + isz,
            sizeof (arguments) - isz);
        assert (rcp);
    }
    isz = strlen (arguments);
    zmq_snprintf (arguments + isz, sizeof (arguments) - isz, ":%d",
        (int) ntohs (ip_address.sin_port));

    //  Listen for incomming connections.
    rc = listen (s, 10);
    errno_assert (rc == 0);

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_sctp_listener_t::~bp_sctp_listener_t ()
{
    //  Cleanup the socket.
    int rc = ::close (s);
    errno_assert (rc == 0);
}

zmq::engine_type_t zmq::bp_sctp_listener_t::type ()
{
    return engine_type_fd;
}

void zmq::bp_sctp_listener_t::get_watermarks (uint64_t *hwm_, uint64_t *lwm_)
{
    //  There are never pipes created to/from listener engine.
    //  Thus, watermarks have no meaning.
    assert (false);
}

void zmq::bp_sctp_listener_t::register_event (i_poller *poller_)
{
    handle_t handle = poller_->add_fd (s, this);
    poller_->set_pollin (handle);
}

void zmq::bp_sctp_listener_t::in_event ()
{
    //  Create the engine to take care of the connection.
    //  TODO: make buffer size configurable by user
    bp_sctp_engine_t *engine = bp_sctp_engine_t::create (thread,
        handler_threads [current_handler_thread], s, peer_name);
    assert (engine);

    if (source) {

        //  The newly created engine serves as a local source of messages
        //  I.e. it reads messages from the socket and passes them on to
        //  the peer engine.
        i_thread *source_thread = handler_threads [current_handler_thread];
        i_engine *source_engine = engine;

        //  Create the pipe to the newly created engine.
        pipe_t *pipe = new pipe_t (source_thread, source_engine,
            peer_thread, peer_engine);
        assert (pipe);

        //  Bind new engine to the source end of the pipe.
        command_t cmd_send_to;
        cmd_send_to.init_engine_send_to (source_engine, "", pipe);
        thread->send_command (source_thread, cmd_send_to);

        //  Bind the peer to the destination end of the pipe.
        command_t cmd_receive_from;
        cmd_receive_from.init_engine_receive_from (peer_engine,
            peer_name, pipe);
        thread->send_command (peer_thread, cmd_receive_from);
    }
    else {

        //  The newly created engine serves as a local destination of messages
        //  I.e. it sends messages received from the peer engine to the socket.
        i_thread *destination_thread =
            handler_threads [current_handler_thread];
        i_engine *destination_engine = engine;

        //  Create the pipe to the newly created engine.
        pipe_t *pipe = new pipe_t (peer_thread, peer_engine,
            destination_thread, destination_engine);
        assert (pipe);

        //  Bind new engine to the destination end of the pipe.
        command_t cmd_receive_from;
        cmd_receive_from.init_engine_receive_from (
            destination_engine, "", pipe);
        thread->send_command (destination_thread, cmd_receive_from);

        //  Bind the peer to the source end of the pipe.
        command_t cmd_send_to;
        cmd_send_to.init_engine_send_to (peer_engine, peer_name, pipe);
        thread->send_command (peer_thread, cmd_send_to);
    }

    //  Move to the next thread to get round-robin balancing of engines.
    current_handler_thread ++;
    if (current_handler_thread == handler_threads.size ())
        current_handler_thread = 0;
}

void zmq::bp_sctp_listener_t::out_event ()
{
    //  We will never get POLLOUT when listening for incoming connections.
    assert (false);
}

void zmq::bp_sctp_listener_t::unregister_event ()
{
    //  TODO: implement this
    assert (false);
}

void zmq::bp_sctp_listener_t::process_command (const engine_command_t &command_)
{
    //  TODO: The only event handled here should be terminate, which would
    //  release the object (delete this)
    assert (false);
}

const char *zmq::bp_sctp_listener_t::get_arguments ()
{
    return arguments;
}

#endif
