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

#include "amqp09_listener.hpp"
#include "amqp09_server_engine.hpp"

zmq::amqp09_listener_t *zmq::amqp09_listener_t::create (poll_thread_t *thread_,
    const char *interface_, uint16_t port_, int handler_thread_count_,
    poll_thread_t **handler_threads_, locator_t *locator_)
{
    amqp09_listener_t *instance = new amqp09_listener_t (thread_,
        interface_, port_, handler_thread_count_, handler_threads_, locator_);
    assert (instance);
    return instance;
}

zmq::amqp09_listener_t::amqp09_listener_t (poll_thread_t *thread_,
      const char *interface_, uint16_t port_, int handler_thread_count_,
      poll_thread_t **handler_threads_, locator_t *locator_) :
    context (thread_),
    locator (locator_),
    listener (interface_, port_)
{
    //  Initialise the array of threads to handle new connections
    assert (handler_thread_count_ > 0);
    for (int thread_nbr = 0; thread_nbr != handler_thread_count_; thread_nbr ++)
        handler_threads.push_back (handler_threads_ [thread_nbr]);
    current_handler_thread = 0;

    //  Register the listener with the polling thread
    thread_->register_engine (this);
}

zmq::amqp09_listener_t::~amqp09_listener_t ()
{
}

int zmq::amqp09_listener_t::get_fd ()
{
    return listener.get_fd ();
}

short zmq::amqp09_listener_t::get_events ()
{
    return POLLIN;
}

bool zmq::amqp09_listener_t::in_event ()
{
    //  Create the engine to take care of the socket
    //  TODO: make buffer size configurable by user
    amqp09_server_engine_t *engine = amqp09_server_engine_t::create (
        handler_threads [current_handler_thread],
        listener, 8192, 8192, locator);
    assert (engine);

    //  Move to the next thread to get round-robin balancing of engines
    current_handler_thread ++;
    if (current_handler_thread == handler_threads.size ())
        current_handler_thread = 0;
    return true;
}

bool zmq::amqp09_listener_t::out_event ()
{
    //  We should never get POLLOUT when listening for incoming connections
    assert (false);
    return true;
}

void zmq::amqp09_listener_t::close_event ()
{
    //  TODO: engine tear-down
    assert (false);
}

void zmq::amqp09_listener_t::process_command (const engine_command_t &command_)
{
    // Listener doesn't expect any commands
    assert (false);
}
