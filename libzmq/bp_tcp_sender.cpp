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

#include <zmq/bp_tcp_sender.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>

zmq::bp_tcp_sender_t::bp_tcp_sender_t (i_mux *mux_, const char *hostname_,
      const char *local_object_, const char * /* options_*/) :
    mux (mux_),
    writebuf_size (bp_out_batch_size),
    write_size (0),
    write_pos (0),
    encoder (mux),
    poller (NULL),
    local_object (local_object_),
    reconnect_flag (true),
    state (engine_connecting),
    socket (hostname_)
{
    zmq_assert (mux);

    //  Allocate write buffer.
    writebuf = new unsigned char [writebuf_size];
    zmq_assert (writebuf);
}

zmq::bp_tcp_sender_t::bp_tcp_sender_t (i_mux *mux_, fd_t fd_,
      const char *local_object_) :
    mux (mux_),
    writebuf_size (bp_out_batch_size),
    write_size (0),
    write_pos (0),
    encoder (mux),
    poller (NULL),
    local_object (local_object_),
    reconnect_flag (false),
    state (engine_connected),
    socket (fd_)
{
    zmq_assert (mux);

    //  Allocate write buffer.
    writebuf = new unsigned char [writebuf_size];
    zmq_assert (writebuf);
}

zmq::bp_tcp_sender_t::~bp_tcp_sender_t ()
{
    delete [] writebuf;
    //  TODO: For engines with identity mux should not be deleted.
    delete mux;
}

void zmq::bp_tcp_sender_t::start (i_thread *current_thread_,
    i_thread *engine_thread_)
{
    mux->register_engine (this);

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    current_thread_->send_command (engine_thread_, command);
}

void zmq::bp_tcp_sender_t::error ()
{
    if (state == engine_connected) {

        //  Clean half-processed outbound data.
        encoder.reset ();
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

void zmq::bp_tcp_sender_t::reconnect ()
{
    if (state == engine_connected || state == engine_connecting) {

        //  Stop polling the socket.
        poller->rm_fd (handle);

        //  Close the socket.
        socket.close ();

        //  Clear data buffers.
        write_pos = write_size;
    }

    //  This is the case when we've tried to reconnect but the attmpt have
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

void zmq::bp_tcp_sender_t::shutdown ()
{
    //  Remove the file descriptor from the pollset.
    poller->rm_fd (handle);

    //  We don't need the socket any more, so close it to allow OS to reuse it.
    socket.close ();

    //  Ask all inbound pipes to shut down.
    mux->initialise_shutdown ();

    state = engine_shutting_down;
}

zmq::i_pollable *zmq::bp_tcp_sender_t::cast_to_pollable ()
{
    return this;
}

void zmq::bp_tcp_sender_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

zmq::i_demux *zmq::bp_tcp_sender_t::get_demux ()
{
    zmq_assert (false);
    return NULL;
}

zmq::i_mux *zmq::bp_tcp_sender_t::get_mux ()
{
    return mux;
}

void zmq::bp_tcp_sender_t::register_event (i_poller *poller_)
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

void zmq::bp_tcp_sender_t::in_event ()
{
    //  Following code should be invoked when async connect causes POLLERR
    //  rather than POLLOUT.
    if (state == engine_connecting)
        zmq_assert (socket.socket_error ());

    // Is connection down?
    else {
        char tmpbuf [1];
        int n = socket.read (tmpbuf, sizeof tmpbuf);
        zmq_assert (n == -1);
    }

    error ();
}

void zmq::bp_tcp_sender_t::out_event ()
{
    if (state == engine_connecting) {

        if (socket.socket_error ()) {
            error ();
            return;
        }

        if (mux->empty ())
            poller->reset_pollout (handle);
        poller->set_pollin (handle);
        state = engine_connected;
        return;
    }

    //  If write buffer is empty, try to read new data from the encoder.
    if (write_pos == write_size) {

        write_size = encoder.read (writebuf, writebuf_size);
        write_pos = 0;

        //  If there is no data to send, stop polling for output.
        if (write_size == 0)
            poller->reset_pollout (handle);
    }

    //  If there are any data to write in write buffer, write as much as
    //  possible to the socket.
    if (write_pos < write_size) {
        int nbytes = socket.write (writebuf + write_pos,
            write_size - write_pos);

        //  Handle problems with the connection.
        if (nbytes == -1) {
            error ();
            return;
        }

        write_pos += nbytes;
    }
}

void zmq::bp_tcp_sender_t::timer_event ()
{
    zmq_assert (state == engine_waiting_for_reconnect);
    reconnect ();
}

void zmq::bp_tcp_sender_t::unregister_event ()
{
    //  TODO: Implement full-blown shut-down mechanism.
    //  For now, we'll just close the underlying socket.
    if (state != engine_waiting_for_reconnect &&
          state != engine_shutting_down) {
        poller->rm_fd (handle);
        socket.close ();
    }
}

void zmq::bp_tcp_sender_t::revive ()
{
    //  Don't start polling for output if you are not connected.
    if (state == engine_connected) {

        //  There is at least one engine that has messages ready. Try to
        //  write data to the socket, thus eliminating one polling
        //  for POLLOUT event.
        if (write_size == 0) {
            poller->set_pollout (handle);
            out_event ();
        }
    }
}

void zmq::bp_tcp_sender_t::receive_from (pipe_t *pipe_)
{
    mux->receive_from (pipe_);

    //  If we are already in shut down phase, initiate shut down of the pipe
    //  immediately.
    if (state == engine_shutting_down)
        pipe_->terminate_reader ();

    if (state == engine_connected)
        poller->set_pollout (handle);
}

const char *zmq::bp_tcp_sender_t::get_arguments ()
{
    zmq_assert (false);
    return NULL;
}

void zmq::bp_tcp_sender_t::head ()
{
    zmq_assert (false);
}

void zmq::bp_tcp_sender_t::send_to (pipe_t *pipe_)
{
    zmq_assert (false);
}
