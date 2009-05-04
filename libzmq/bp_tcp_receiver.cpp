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

#include <zmq/bp_tcp_receiver.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>
#include <zmq/i_demux.hpp>

zmq::bp_tcp_receiver_t::bp_tcp_receiver_t (i_demux *demux_, 
      i_thread *calling_thread_, i_thread *thread_, const char *hostname_,
      const char *local_object_, const char * /* options_*/) :
    demux (demux_),
    readbuf_size (bp_in_batch_size),
    read_size (0),
    read_pos (0),
    decoder (demux),
    poller (NULL),
    local_object (local_object_),
    reconnect_flag (true),
    state (engine_connecting),
    socket (hostname_)
{

    zmq_assert (demux);

    //  Allocate read buffer.
    readbuf = new unsigned char [readbuf_size];
    zmq_assert (readbuf);

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_tcp_receiver_t::bp_tcp_receiver_t (i_demux *demux_, 
      i_thread *calling_thread_, i_thread *thread_, tcp_listener_t &listener_,
      const char *local_object_) :
    demux (demux_),
    readbuf_size (bp_in_batch_size),
    read_size (0),
    read_pos (0),
    decoder (demux),
    poller (NULL),
    local_object (local_object_),
    reconnect_flag (false),
    state (engine_connected),
    socket (listener_)
{

    zmq_assert (demux);

    //  Allocate read buffer.
    readbuf = new unsigned char [readbuf_size];
    zmq_assert (readbuf);

    //  Register BP/TCP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_tcp_receiver_t::~bp_tcp_receiver_t ()
{
    delete [] readbuf;
    //  TODO: For engines with identity demux should not be deleted.
    delete demux;
}

void zmq::bp_tcp_receiver_t::error ()
{
    if (state == engine_connected) {

        //  Push a gap notification to the pipes.
        demux->gap ();

        //  Clean half-processed inbound data.
        decoder.reset ();
    }

    //  Report connection failure to the client.
    //  If there is no error handler registered, continue quietly.
    //  If error handler returns true, continue quietly.
    //  If error handler returns false, crash the application.
    error_handler_t *eh = get_error_handler ();
    if (eh && !eh (local_object.c_str ()))
        zmq_assert (false);

    //  Either reestablish the connection or destroy associated resources.
    if (reconnect_flag)
        reconnect ();
    else
        shutdown ();
}

void zmq::bp_tcp_receiver_t::reconnect ()
{
    if (state == engine_connected || state == engine_connecting) {

        //  Stop polling the socket.
        poller->rm_fd (handle);

        //  Close the socket.
        socket.close ();

        //  Clear data buffer.
        read_pos = read_size;
    }

    //  This is the case when we've tried to reconnect but the attempt have
    //  failed. We are going to wait a while before trying to reconnect anew
    //  to prevent reconnect consuming 100% of the processor time.
    if (state == engine_connecting) {
        poller->add_timer (this);
        state = engine_waiting_for_reconnect;
        return;
    }

    //  Reopen the socket. This initiates the TCP connection establishment.
    //  If the reconnection is unsuccessfull wait a while till attempting
    //  it anew.
    socket.reopen ();
    if (socket.get_fd () == retired_fd) {
        poller->add_timer (this);
        state = engine_waiting_for_reconnect;
        return;
    }

    //  The output event is used to signal that we can get
    //  the connection status. Register our interest in it.
    handle = poller->add_fd (socket.get_fd (), this);
    poller->set_pollout (handle);

    state = engine_connecting;
}

void zmq::bp_tcp_receiver_t::shutdown ()
{
    //  Remove the file descriptor from the pollset.
    poller->rm_fd (handle);

    //  We don't need the socket any more, so close it to allow OS to reuse it.
    socket.close ();

    //  Ask all outbound pipes to shut down.
    demux->initialise_shutdown ();

    state = engine_shutting_down;
}

zmq::i_pollable *zmq::bp_tcp_receiver_t::cast_to_pollable ()
{
    return this;
}

void zmq::bp_tcp_receiver_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

int64_t zmq::bp_tcp_receiver_t::get_swap_size ()
{
    return 0;
}

void zmq::bp_tcp_receiver_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  If initial attemp to connect failed, schedule reconnect.
    if (socket.get_fd () == retired_fd) {
        poller->add_timer (this);
        state = engine_waiting_for_reconnect;
        return;
    }

    //  Initialise the poll handle.
    handle = poller->add_fd (socket.get_fd (), this);

    if (state == engine_connecting)
        //  Wait for completion of connect() call.
        poller->set_pollout (handle);
    else
        //  Start receiving 'backend protocol' messages.
        poller->set_pollin (handle);
}

void zmq::bp_tcp_receiver_t::in_event ()
{
    //  Following code should be invoked when async connect causes POLLERR
    //  rather than POLLOUT.
    if (state == engine_connecting) {
        zmq_assert (socket.socket_error ());
        error ();
        return;
    }

    //  If there's no data to process in the buffer, read new data.
    if (read_pos == read_size) {

        //  Read as much data as possible to the read buffer.
        read_size = socket.read (readbuf, readbuf_size);
        read_pos = 0;

        //  Check whether the peer has closed the connection.
        if (read_size == -1) {
            error ();
            return;
        }
    }

    //  If there's at least one unprocessed byte in the buffer, process it.
    if (read_pos < read_size) {

        //  Push the data to the decoder.
        int nbytes = decoder.write (readbuf + read_pos, read_size - read_pos);

        //  If at least one byte was processed, flush all messages the decoder
        //  may have produced.
        if (nbytes > 0)
            demux->flush ();

        //  Adjust read position. Stop monitoring input if we got stuck.
        read_pos += nbytes;
        if (read_pos < read_size)
            poller->reset_pollin (handle);
    }
}

void zmq::bp_tcp_receiver_t::out_event ()
{
    zmq_assert (state == engine_connecting);

    if (socket.socket_error ())
        error ();
    else {
        poller->reset_pollout (handle);
        poller->set_pollin (handle);
        state = engine_connected;
    }
}

void zmq::bp_tcp_receiver_t::timer_event ()
{
    zmq_assert (state == engine_waiting_for_reconnect);
    reconnect ();
}

void zmq::bp_tcp_receiver_t::unregister_event ()
{
    //  TODO: Implement full-blown shut-down mechanism.
    //  For now, we'll just close the underlying socket.
    if (state != engine_waiting_for_reconnect &&
          state != engine_shutting_down) {
        poller->rm_fd (handle);
        socket.close ();
    }
}

void zmq::bp_tcp_receiver_t::head (pipe_t *pipe_, int64_t position_)
{
    if (read_pos < read_size) {

        //  Decoding has been suspended - try to resume it now.
        int nbytes = decoder.write (readbuf + read_pos, read_size - read_pos);

        //  Flush pending messages if necessary.
        if (nbytes > 0)
            demux->flush ();

        //  Has the message decoder consumed all the data? If so, we need to
        //  get notified when more input data is available.
        read_pos += nbytes;
        if (read_pos == read_size)
            poller->set_pollin (handle);
    }
    else {

        //  It's possible there is a message left in the decoder.
        //  Give the decoder opportunity to push the message forward.
        //
        //  TODO: the flush operation can be rather expensive so we
        //        should call it only when absolutely necessary.
        decoder.write (NULL, 0);
        demux->flush ();
    }
}

void zmq::bp_tcp_receiver_t::send_to (pipe_t *pipe_)
{
    //  If pipe limits are set, POLLIN may be turned off
    //  because there are no pipes to send messages to.
    //  So, if this is the first pipe in demux, start polling.
    if (state == engine_connected && demux->no_pipes ())
        poller->set_pollin (handle);

    demux->send_to (pipe_);
}

const char *zmq::bp_tcp_receiver_t::get_arguments ()
{
    zmq_assert (false);
    return NULL;
}

void zmq::bp_tcp_receiver_t::revive (pipe_t *pipe_)
{
    zmq_assert (false);
}

void zmq::bp_tcp_receiver_t::receive_from (pipe_t *pipe_)
{
    zmq_assert (false);
}

void zmq::bp_tcp_receiver_t::terminate_pipe (pipe_t *pipe_)
{
    //  Drop reference to the pipe.
    demux->release_pipe (pipe_);
}

void zmq::bp_tcp_receiver_t::terminate_pipe_ack (pipe_t *pipe_)
{
    zmq_assert (false);
}

