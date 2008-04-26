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

#include "amqp09_listener.hpp"
#include "amqp09_server_engine.hpp"

zmq::amqp09_listener_t *zmq::amqp09_listener_t::create (
    i_thread *handler_thread_, const char *interface_, uint16_t port_)
{
    amqp09_listener_t *instance = new amqp09_listener_t (handler_thread_,
        interface_, port_);
    assert (instance);
    return instance;
}

zmq::amqp09_listener_t::amqp09_listener_t (i_thread *handler_thread_,
      const char *interface_, uint16_t port_) :
    thread (NULL),
    handler_thread (handler_thread_)
{
    //  Create IP addess
    sockaddr_in interface;
    memset (&interface, 0, sizeof (interface));
    interface.sin_family = AF_INET;
    int rc = inet_pton (AF_INET, interface_, &interface.sin_addr);
    errno_assert (rc > 0);
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
}

zmq::amqp09_listener_t::~amqp09_listener_t ()
{
    //  Close the listening socket
    int rc = close (sock);
    errno_assert (rc == 0);
}

void zmq::amqp09_listener_t::set_thread (i_thread *thread_)
{
    thread = thread_;
}

int zmq::amqp09_listener_t::get_fd ()
{
    return sock;
}

short zmq::amqp09_listener_t::get_events ()
{
    return POLLIN;
}

void zmq::amqp09_listener_t::in_event ()
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
    amqp09_server_engine_t *engine = amqp09_server_engine_t::create (
        s, 8192, 8192, "", "", "", "");
    assert (engine);

    //  Plug the engine to the poll thread
    command_t command;
    command.init_register_engine (engine);
    thread->send_command (handler_thread->get_thread_id (), command);
}

void zmq::amqp09_listener_t::out_event ()
{
    //  We should never get POLLOUT when listening for incoming connections
    assert (false);
}

void zmq::amqp09_listener_t::process_command (const engine_command_t &command_)
{
    // Listener doesn't expect any commands
    assert (false);
}
