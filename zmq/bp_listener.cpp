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

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <poll.h>
#include <netdb.h>

#include "bp_listener.hpp"
#include "bp_engine.hpp"

zmq::bp_listener_t *zmq::bp_listener_t::create (poll_thread_t *thread_,
    const char *interface_, uint16_t port_, int handler_thread_count_,
    poll_thread_t **handler_threads_, bool source_,
    i_context *peer_context_, i_engine *peer_engine_,
    const char *peer_name_,
    bool abort_on_close_)
{
    bp_listener_t *instance = new bp_listener_t (thread_, interface_, port_,
        handler_thread_count_, handler_threads_, source_, peer_context_,
        peer_engine_, peer_name_, abort_on_close_);
    assert (instance);

    return instance;
}

zmq::bp_listener_t::bp_listener_t (poll_thread_t *thread_,
      const char *interface_, uint16_t port_, int handler_thread_count_,
      poll_thread_t **handler_threads_, bool source_,
      i_context *peer_context_, i_engine *peer_engine_,
      const char *peer_name_, bool abort_on_close_) :
    context (thread_),
    source (source_),
    peer_context (peer_context_),
    peer_engine (peer_engine_),
    abort_on_close (abort_on_close_)
{
    //  Copy the peer name
    assert (strlen (peer_name_) < 16);
    strcpy (peer_name, peer_name_);

    //  Initialise the array of threads to handle new connections
    assert (handler_thread_count_ > 0);
    for (int thread_nbr = 0; thread_nbr != handler_thread_count_; thread_nbr ++)
        handler_threads.push_back (handler_threads_ [thread_nbr]);
    current_handler_thread = 0;

    //  Create IP addess
    struct addrinfo req;
    memset (&req, 0, sizeof req);
    struct addrinfo *res;
    req.ai_family = AF_INET;
    int rc = getaddrinfo (interface_, NULL, &req, &res);
    assert (rc == 0);
    sockaddr_in interface = *((sockaddr_in *)res->ai_addr);
    freeaddrinfo (res);
    interface.sin_port = htons (port_);

    //  Create a listening socket
    sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    errno_assert (sock != -1);

    //  Allow socket reusing
    int flag = 1;
    rc = setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Bind the socket to the network interface and port
    rc = bind (sock, (struct sockaddr*) &interface, sizeof (interface));
    errno_assert (rc == 0);
              
    //  Start listening for incomming connections
    rc = ::listen (sock, 1);
    errno_assert (rc == 0); 

    //  Register the listener with the polling thread
    thread_->register_engine (this);   
}

zmq::bp_listener_t::~bp_listener_t ()
{
    //  Close the listening socket
    int rc = close (sock);
    errno_assert (rc == 0);
}

int zmq::bp_listener_t::get_fd ()
{
    return sock;
}

short zmq::bp_listener_t::get_events ()
{
    return POLLIN;
}

bool zmq::bp_listener_t::in_event ()
{
    //  Accept first incoming connection
    int s = accept (sock, NULL, NULL);
    errno_assert (s != -1);

    //  Disable Nagle's algorithm
    int flag = 1;
    int rc = setsockopt (s, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Create the engine to take care of the socket
    //  TODO: make buffer size configurable by user
    bp_engine_t *engine = bp_engine_t::create (
        handler_threads [current_handler_thread], s, 8192, 8192, abort_on_close);
    assert (engine);

    if (source) {

        //  The newly created engine serves as a local source of messages
        //  I.e. it reads messages from the socket and passes them on to
        //  the peer engine.
        i_context *source_context = handler_threads [current_handler_thread];
        i_engine *source_engine = engine;

        //  Create the pipe to the newly created engine
        pipe_t *pipe = new pipe_t (source_context, source_engine,
            peer_context, peer_engine);
        assert (pipe);

        //  Bind new engine to the source end of the pipe
        command_t cmd_send_to;
        cmd_send_to.init_engine_send_to (source_engine, "", pipe);
        source_engine->process_command (
            cmd_send_to.args.engine_command.command);

        //  Bind the peer to the destination end of the pipe
        command_t cmd_receive_from;
        cmd_receive_from.init_engine_receive_from (peer_engine,
            peer_name, pipe);
        context->send_command (peer_context, cmd_receive_from);
    }
    else {

        //  The newly created engine serves as a local destination of messages
        //  I.e. it sends messages received from the peer engine to the socket.
        i_context *destination_context =
            handler_threads [current_handler_thread];
        i_engine *destination_engine = engine;

        //  Create the pipe to the newly created engine
        pipe_t *pipe = new pipe_t (peer_context, peer_engine,
            destination_context, destination_engine);
        assert (pipe);

        //  Bind new engine to the destination end of the pipe
        command_t cmd_receive_from;
        cmd_receive_from.init_engine_receive_from (
            destination_engine, "", pipe);
        destination_engine->process_command (
            cmd_receive_from.args.engine_command.command);

        //  Bind the peer to the source end of the pipe
        command_t cmd_send_to;
        cmd_send_to.init_engine_send_to (peer_engine, peer_name, pipe);
        context->send_command (peer_context, cmd_send_to);
    }

    //  Move to the next thread to get round-robin balancing of engines
    current_handler_thread ++;
    if (current_handler_thread == handler_threads.size ())
        current_handler_thread = 0;
    return true;
}

bool zmq::bp_listener_t::out_event ()
{
    //  We will never get POLLOUT when listening for incoming connections
    assert (false);
    return true;
}

void zmq::bp_listener_t::close_event()
{
    //  TODO: engine tear-down
    assert (false);
}

void zmq::bp_listener_t::process_command (const engine_command_t &command_)
{
    //  TODO: The only event handled here should be terminate, which would
    //  release the object (delete this)
    assert (false);
}
