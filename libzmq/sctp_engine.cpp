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

#include <zmq/sctp_engine.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/ip.hpp>

zmq::sctp_engine_t::sctp_engine_t (i_thread *calling_thread_,
      i_thread *thread_, const char *hostname_, const char *local_object_,
      const char *arguments_) :
    poller (NULL),
    local_object (local_object_),
    shutting_down (false)
{
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

    //  Register the engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::sctp_engine_t::sctp_engine_t (i_thread *calling_thread_,
      i_thread *thread_, int listener_, const char *local_object_) :
    poller (NULL),
    local_object (local_object_),
    shutting_down (false)
{
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

    //  Register SCTP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::sctp_engine_t::~sctp_engine_t ()
{
    //  Cleanup the socket.
    int rc = ::close (s);
    errno_assert (rc == 0);
}

zmq::i_pollable *zmq::sctp_engine_t::cast_to_pollable ()
{
    return this;
}

void zmq::sctp_engine_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

void zmq::sctp_engine_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Initialise the poll handle.
    handle = poller->add_fd (s, this);
    poller->set_pollin (handle);
}

void zmq::sctp_engine_t::in_event ()
{
    //  Receive N messages in one go if possible - this way we'll avoid
    //  excessive polling.
    //  TODO: Move the constant to config.hpp
    int msg_nbr;
    for (msg_nbr = 0; msg_nbr != 1; msg_nbr ++) {

        unsigned char buffer [max_sctp_message_size]; 
        int msg_flags;
        ssize_t nbytes = sctp_recvmsg (s, buffer, sizeof (buffer),
            NULL, 0, NULL, &msg_flags);
        errno_assert (nbytes != -1);

        //  Create 0MQ message from the data.
        message_t msg (nbytes);
        memcpy (msg.data (), buffer, nbytes);

        //  TODO: Implement queue-full handling.
        bool ok = demux.write (msg);
        assert (ok);
    }

    //  Flash the messages to system, if there are any.
    if (msg_nbr > 0)
        demux.flush ();
}

void zmq::sctp_engine_t::out_event ()
{
    message_t msg;
    if (!mux.read (&msg)) {

        //  If there are no messages to send, stop polling for output.
        poller->reset_pollout (handle);
        return;
    }

    //  Send the data over the wire.
    ssize_t nbytes = sctp_sendmsg (s, msg.data (), msg.size (),
        NULL, 0, 0, 0, 0, 0, 0);
    errno_assert (nbytes != -1);
    assert (nbytes == (ssize_t) msg.size ());
}

void zmq::sctp_engine_t::unregister_event ()
{
    //  TODO: Implement this.
    assert (false);
}

void zmq::sctp_engine_t::revive (pipe_t *pipe_)
{
    if (!shutting_down) {

        engine_base_t <true,true>::revive (pipe_);

        //  There is at least one engine that has messages ready. Try to
        //  write data to the socket, thus eliminating one polling
        //  for POLLOUT event.
        poller->set_pollout (handle);
        out_event ();
    }
}

void zmq::sctp_engine_t::head (pipe_t *pipe_, int64_t position_)
{
    engine_base_t <true,true>::head (pipe_, position_);
    in_event ();
}

void zmq::sctp_engine_t::send_to (pipe_t *pipe_)
{
    if (!shutting_down) {

        //  If pipe limits are set, POLLIN may be turned off
        //  because there are no pipes to send messages to.
        //  So, if this is the first pipe in demux, start polling.
        if (demux.no_pipes ())
            poller->set_pollin (handle);

        //  Start sending messages to a pipe.
        engine_base_t <true,true>::send_to (pipe_);
    }
}

void zmq::sctp_engine_t::receive_from (pipe_t *pipe_)
{
    //  Start receiving messages from a pipe.
    engine_base_t <true,true>::receive_from (pipe_);
    if (shutting_down)
        pipe_->terminate_reader ();
    else
        poller->set_pollout (handle);
}

#endif
