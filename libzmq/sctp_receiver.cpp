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

#include <zmq/sctp_receiver.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/i_demux.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/ip.hpp>

zmq::sctp_receiver_t::sctp_receiver_t (i_demux *demux_, const char *hostname_,
      const char *local_object_, const char * /* arguments_ */) :
    demux (demux_),
    poller (NULL),
    local_object (local_object_),
    shutting_down (false)
{
    zmq_assert (demux);

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

zmq::sctp_receiver_t::sctp_receiver_t (i_demux *demux_,
      int listener_, const char *local_object_):
    demux (demux_),
    poller (NULL),
    local_object (local_object_),
    shutting_down (false)
{
    zmq_assert (demux);

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

zmq::sctp_receiver_t::~sctp_receiver_t ()
{
}

void zmq::sctp_receiver_t::start (i_thread *current_thread,
    i_thread *engine_thread_)
{
    demux->register_engine (this);

    //  Register the engine with the I/O thread.
    command_t command;
    command.init_register_pollable (this);
    current_thread->send_command (engine_thread_, command);
}

zmq::i_demux *zmq::sctp_receiver_t::get_demux ()
{
    return demux;
}

zmq::i_mux *zmq::sctp_receiver_t::get_mux ()
{
    zmq_assert (false);
    return NULL;
}

void zmq::sctp_receiver_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Initialise the poll handle.
    handle = poller->add_fd (s, this);
    poller->set_pollin (handle);
}

void zmq::sctp_receiver_t::in_event ()
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
        bool ok = demux->write (msg);
        zmq_assert (ok);
    }

    //  Flash the messages to system, if there are any.
    if (msg_nbr > 0)
        demux->flush ();
}

void zmq::sctp_receiver_t::out_event ()
{
    zmq_assert (false);
}

void zmq::sctp_receiver_t::timer_event ()
{
    //  We are setting no timers. We shouldn't get this event.
    zmq_assert (false);
}

void zmq::sctp_receiver_t::unregister_event ()
{
    //  TODO: Implement this. For now we'll do nothing here.
}

void zmq::sctp_receiver_t::revive ()
{
    zmq_assert (false);
}

void zmq::sctp_receiver_t::head ()
{
    in_event ();
}

void zmq::sctp_receiver_t::send_to ()
{
    if (!shutting_down) {

        //  If pipe limits are set, POLLIN may be turned off
        //  because there are no pipes to send messages to.
        //  So, if this is the first pipe in demux, start polling.
        if (demux->no_pipes ())
            poller->set_pollin (handle);
    }
}

void zmq::sctp_receiver_t::receive_from ()
{
    zmq_assert (false);
}

const char *zmq::sctp_receiver_t::get_arguments ()
{
    zmq_assert (false);
    return NULL;
}

#endif
