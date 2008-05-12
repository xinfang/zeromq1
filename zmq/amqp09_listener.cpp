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

zmq::amqp09_listener_t *zmq::amqp09_listener_t::create (poll_thread_t *thread_,
    const char *interface_, uint16_t port_, int handler_thread_count_,
    poll_thread_t **handler_threads_)
{
    amqp09_listener_t *instance = new amqp09_listener_t (thread_,
        interface_, port_, handler_thread_count_, handler_threads_);
    assert (instance);
    return instance;
}

zmq::amqp09_listener_t::amqp09_listener_t (poll_thread_t *thread_,
      const char *interface_, uint16_t port_, int handler_thread_count_,
      poll_thread_t **handler_threads_) :
    context (thread_)
{
    //  Initialise the array of threads to handle new connections
    assert (handler_thread_count_ > 0);
    for (int thread_nbr = 0; thread_nbr != handler_thread_count_; thread_nbr ++)
        handler_threads.push_back (handler_threads_ [thread_nbr]);
    current_handler_thread = 0;

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

    //  Register the listener with the polling thread
    thread_->register_engine (this);
}

zmq::amqp09_listener_t::~amqp09_listener_t ()
{
    //  Close the listening socket
    int rc = close (sock);
    errno_assert (rc == 0);
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
        handler_threads [current_handler_thread],
        s, 8192, 8192, "", "", "", "");
    assert (engine);

    //  Move to the next thread to get round-robin balancing of engines
    current_handler_thread ++;
    if (current_handler_thread == handler_threads.size ())
        current_handler_thread = 0;
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
