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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <zmq/sctp_sender.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/ip.hpp>

zmq::sctp_sender_t::sctp_sender_t (mux_t *mux_, const char *hostname_,
      const char *local_object_, const char * /* arguments_ */) :
    mux (mux_),
    poller (NULL),
    local_object (local_object_),
    shutting_down (false)
{
    zmq_assert (mux);

    //  Convert the hostname into sockaddr_in structure.
    sockaddr_in ip_address;
    resolve_ip_hostname (&ip_address, hostname_);

    //  Create a socket.
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    errno_assert (s != -1);

    //  Subscribe for SCTP events.
    sctp_event_subscribe events;
    memset (&events, 0, sizeof (events));
    events.sctp_data_io_event = 1;
    int rc = setsockopt (s, IPPROTO_SCTP, SCTP_EVENTS, &events,
        sizeof (events));
    errno_assert (rc == 0);

    //  Connect to the peer.
    rc = connect (s, (sockaddr*) &ip_address, sizeof (ip_address));
    errno_assert (rc != -1);

    //  Switch off Nagle's algorithm.
    int flag = 1;
    rc = setsockopt (s, IPPROTO_SCTP, SCTP_NODELAY, &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Set to non-blocking mode.
    int flags = fcntl (s, F_GETFL, 0);
    if (flags == -1)
        flags = 0;
    rc = fcntl (s, F_SETFL, flags | O_NONBLOCK);
    errno_assert (rc != -1);
}

zmq::sctp_sender_t::sctp_sender_t (mux_t *mux_,
      int listener_, const char *local_object_) :
    mux (mux_),
    poller (NULL),
    local_object (local_object_),
    shutting_down (false)
{
    zmq_assert (mux);

    //  Accept the incoming connection.
    s = accept (listener_, NULL, NULL);
    errno_assert (s != -1);

    //  Switch off Nagle's algorithm.
    int flag = 1;
    int rc = setsockopt (s, IPPROTO_SCTP, SCTP_NODELAY, &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Set to non-blocking mode.
    int flags = fcntl (s, F_GETFL, 0);
    if (flags == -1)
        flags = 0;
    rc = fcntl (s, F_SETFL, flags | O_NONBLOCK);
    errno_assert (rc != -1);
}

zmq::sctp_sender_t::~sctp_sender_t ()
{
    //  Cleanup the socket.
    int rc = ::close (s);
    errno_assert (rc == 0);
}

void zmq::sctp_sender_t::start (i_thread *current_thread_,
    i_thread *engine_thread_)
{
    mux->register_engine (this);

    //  Register the engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    current_thread_->send_command (engine_thread_, command);
}

zmq::i_pollable *zmq::sctp_sender_t::cast_to_pollable ()
{
    return this;
}

void zmq::sctp_sender_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

int64_t zmq::sctp_sender_t::get_swap_size ()
{
    return 0;
}

zmq::i_demux *zmq::sctp_sender_t::get_demux ()
{
    zmq_assert (false);
    return NULL;
}

zmq::i_mux *zmq::sctp_sender_t::get_mux ()
{
    return mux;
}

void zmq::sctp_sender_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Initialise the poll handle.
    handle = poller->add_fd (s, this);
    poller->set_pollin (handle);
}

void zmq::sctp_sender_t::in_event ()
{
    zmq_assert (false);
}

void zmq::sctp_sender_t::out_event ()
{

    message_t msg;
    if (!mux->read (&msg)) {

        //  If there are no messages to send, stop polling for output.
        poller->reset_pollout (handle);
        return;
    }

    //  Send the data over the wire.
    ssize_t nbytes = sctp_sendmsg (s, msg.data (), msg.size (),
        NULL, 0, 0, 0, 0, 0, 0);
    errno_assert (nbytes != -1);
    zmq_assert (nbytes == (ssize_t) msg.size ());
}

void zmq::sctp_sender_t::timer_event ()
{
    //  We are setting no timers. We shouldn't get this event.
    zmq_assert (false);
}

void zmq::sctp_sender_t::unregister_event ()
{
    //  TODO: Implement this. For now we'll do nothing here.
}

void zmq::sctp_sender_t::revive ()
{
    if (!shutting_down) {

        //  There is at least one engine that has messages ready. Try to
        //  write data to the socket, thus eliminating one polling
        //  for POLLOUT event.
        poller->set_pollout (handle);
        out_event ();
    }
}

void zmq::sctp_sender_t::head ()
{
    zmq_assert (false);
}

void zmq::sctp_sender_t::send_to (pipe_t *pipe_)
{
    zmq_assert (false);
}

void zmq::sctp_sender_t::receive_from (pipe_t *pipe_)
{
    //  Start receiving messages from a pipe.
    mux->receive_from (pipe_);
    if (shutting_down)
        pipe_->terminate_reader ();
    else
        poller->set_pollout (handle);
}

const char *zmq::sctp_sender_t::get_arguments ()
{
    zmq_assert (false);
    return NULL;
}

#endif
