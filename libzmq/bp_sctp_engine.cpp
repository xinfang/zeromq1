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

#include <fcntl.h>

#include <zmq/bp_sctp_engine.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/ip.hpp>

zmq::bp_sctp_engine_t *zmq::bp_sctp_engine_t::create (i_thread *calling_thread_,
    i_thread *thread_, const char *hostname_, const char *local_object_)
{
    bp_sctp_engine_t *instance = new bp_sctp_engine_t (calling_thread_,
        thread_, hostname_, local_object_);
    assert (instance);

    return instance;
}

zmq::bp_sctp_engine_t *zmq::bp_sctp_engine_t::create (i_thread *calling_thread_,
    i_thread *thread_, int listener_, const char *local_object_)
{
    bp_sctp_engine_t *instance = new bp_sctp_engine_t (calling_thread_,
        thread_, listener_, local_object_);
    assert (instance);

    return instance;
}

zmq::bp_sctp_engine_t::bp_sctp_engine_t (i_thread *calling_thread_,
      i_thread *thread_, const char *hostname_, const char *local_object_) :
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

zmq::bp_sctp_engine_t::bp_sctp_engine_t (i_thread *calling_thread_,
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

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_sctp_engine_t::~bp_sctp_engine_t ()
{
    //  Cleanup the socket.
    int rc = ::close (s);
    errno_assert (rc == 0);
}

zmq::engine_type_t zmq::bp_sctp_engine_t::type ()
{
    return engine_type_fd;
}

void zmq::bp_sctp_engine_t::get_watermarks (uint64_t *hwm_, uint64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

void zmq::bp_sctp_engine_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Initialise the poll handle.
    handle = poller->add_fd (s, this);
    poller->set_pollin (handle);
}

void zmq::bp_sctp_engine_t::in_event ()
{
    //  Receive N messages in one go if possible - this way we'll avoid
    //  excessive polling.
    //  TODO: Move the constant to config.hpp
    int msg_nbr;
    for (msg_nbr = 0; msg_nbr != 1; msg_nbr ++) {

            //  TODO: How do we know whether buffer overflow occured?
            //  How can we read messages larger than N bytes?
            //  TODO: Make this non-blocking...
            unsigned char buffer [4096]; 
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

void zmq::bp_sctp_engine_t::out_event ()
{
    //  Send N messages in one go if possible - this way we'll avoid
    //  excessive polling.
    //  TODO: Move the constant to config.hpp
    for (int msg_nbr = 0; msg_nbr != 100; msg_nbr ++) {
        message_t msg;
        if (!mux.read (&msg)) {

            //  If there are no messages to send, stop polling for output.
            poller->reset_pollout (handle);
            return;
        }

        //  Send the data over the wire.
        //  TODO: This call should not block. Investigate the behaviour...
        ssize_t nbytes = sctp_sendmsg (s, msg.data (), msg.size (),
            NULL, 0, 0, 0, 0, 0, 0);
        errno_assert (nbytes != -1);
        assert (nbytes == (ssize_t) msg.size ());
    }
}

void zmq::bp_sctp_engine_t::unregister_event ()
{
    //  TODO: Implement this.
    assert (false);
}

void zmq::bp_sctp_engine_t::process_command (const engine_command_t &command_)
{
    switch (command_.type) {
    case engine_command_t::revive:

        if (!shutting_down) {

            //  Forward the revive command to the pipe.
            command_.args.revive.pipe->revive ();

            //  There is at least one engine that has messages ready. Try to
            //  write data to the socket, thus eliminating one polling
            //  for POLLOUT event.
            poller->set_pollout (handle);
            out_event ();
        }
        break;

    case engine_command_t::head:

        //  Forward pipe statistics to the appropriate pipe.
        if (!shutting_down) {
            command_.args.head.pipe->set_head (command_.args.head.position);
            in_event ();
        }
        break;

    case engine_command_t::send_to:

        if (!shutting_down) {

            //  If pipe limits are set, POLLIN may be turned off
            //  because there are no pipes to send messages to.
            //  So, if this is the first pipe in demux, start polling.
            if (demux.no_pipes ())
                poller->set_pollin (handle);

            //  Start sending messages to a pipe.
            demux.send_to (command_.args.send_to.pipe);
        }
        break;

    case engine_command_t::receive_from:

        //  Start receiving messages from a pipe.
        mux.receive_from (command_.args.receive_from.pipe, shutting_down);
        if (!shutting_down)
            poller->set_pollout (handle);
        break;

    case engine_command_t::terminate_pipe:

        //  Forward the command to the pipe.
        command_.args.terminate_pipe.pipe->writer_terminated ();

        //  Remove all references to the pipe.
        demux.release_pipe (command_.args.terminate_pipe.pipe);
        break;

    case engine_command_t::terminate_pipe_ack:

        //  Forward the command to the pipe.
        command_.args.terminate_pipe_ack.pipe->reader_terminated ();

        //  Remove all references to the pipe.
        mux.release_pipe (command_.args.terminate_pipe_ack.pipe);
        break;

    default:
        assert (false);
    }
}

#endif
