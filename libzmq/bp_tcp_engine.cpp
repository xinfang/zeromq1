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

#include <zmq/bp_tcp_engine.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>

zmq::bp_tcp_engine_t::bp_tcp_engine_t (i_thread *calling_thread_,
      i_thread *thread_, const char *hostname_, const char *local_object_,
      const char *remote_object_, const char * /* arguments_*/) :
    writebuf_size (bp_out_batch_size),
    write_size (0),
    write_pos (0),
    readbuf_size (bp_in_batch_size),
    read_size (0),
    read_pos (0),
    encoder (&mux, true, local_object_),
    decoder (&demux, NULL, false, NULL),
    poller (NULL),
    local_object (local_object_),
    remote_object (remote_object_),
    reconnect_flag (true),
    hostname (hostname_),
    state (engine_connecting)
{
    //  Allocate read and write buffers.
    writebuf = (unsigned char*) malloc (writebuf_size);
    errno_assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    errno_assert (readbuf);

    //  Set remote_object in mux.
    mux.set_remote_object (remote_object);

    //  Open the underlying socket.
    socket = new tcp_socket_t (hostname.c_str ());
    assert (socket);

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_tcp_engine_t::bp_tcp_engine_t (i_thread *calling_thread_,
      i_thread *thread_, tcp_listener_t &listener_, const char *local_object_) :
    writebuf_size (bp_out_batch_size),
    write_size (0),
    write_pos (0),
    readbuf_size (bp_in_batch_size),
    read_size (0),
    read_pos (0),
    encoder (&mux, false, ""),
    decoder (&demux, &mux, true, &remote_object),
    poller (NULL),
    local_object (local_object_),
    reconnect_flag (false),
    state (engine_connected)
{
    //  Allocate read and write buffers.
    writebuf = (unsigned char*) malloc (writebuf_size);
    errno_assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    errno_assert (readbuf);

    //  Open the underlying socket by accepting it from listener.
    socket = new tcp_socket_t (listener_);
    assert (socket);

    //  Register BP/TCP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_tcp_engine_t::~bp_tcp_engine_t ()
{
    delete socket;
    free (readbuf);
    free (writebuf);
}

void zmq::bp_tcp_engine_t::error ()
{
    if (state == engine_connected) {

        //  Push a gap notification to the pipe.
        //  TODO: Handle the case where the pipe is full.
        raw_message_t msg;
        raw_message_init_notification (&msg, raw_message_t::gap_tag);
        if (!demux.write ((message_t&) msg))
            assert (false);
        demux.flush ();

        //  Clean half-processed inbound and outbound data.
        encoder.reset ();
        decoder.reset ();
    }

    //  Report connection failure to the client.
    //  If there is no error handler registered, continue quietly.
    //  If error handler returns true, continue quietly.
    //  If error handler returns false, crash the application.
    error_handler_t *eh = get_error_handler ();

    // DG: 2009/02/17
    // if error handler returns false then initiate shutdown .. otherwise reconnect
    if (eh && !eh (local_object.c_str (), remote_object.c_str ()))
        shutdown ();
    else
        reconnect ();

/*
    if (eh && !eh (local_object.c_str (), remote_object.c_str ()))
        assert (false);

    //  Either reestablish the connection or destroy associated resources.
    if (reconnect_flag)
        reconnect ();
    else
        shutdown ();
*/
}

void zmq::bp_tcp_engine_t::reconnect ()
{
    //  Stop polling the socket.
    poller->rm_fd (handle);

    //  Clear data buffers.
    read_pos = read_size;
    write_pos = write_size;

    //  Destroy the existing socket and create a new one. This
    //  initiates the TCP connection establishment.
    delete socket;

    //  Wait 1s before reconnecting.
#ifdef ZMQ_HAVE_WINDOWS
    Sleep (1000);
#else
    sleep (1);
#endif

    socket = new tcp_socket_t (hostname.c_str ());
    assert (socket);

    //  The output event is used to signal that we can get
    //  the connection status. Register our interest in it.
    handle = poller->add_fd (socket->get_fd (), this);
    poller->set_pollout (handle);

    state = engine_connecting;
}

void zmq::bp_tcp_engine_t::shutdown ()
{
    //  Remove the file descriptor from the pollset.
    poller->rm_fd (handle);

    // We don't need the socket any more, so close it to allow OS to reuse it.
    socket->close ();

    //  Ask all inbound & outound pipes to shut down.
    demux.initialise_shutdown ();
    mux.initialise_shutdown ();

    state = engine_shutting_down;
}

zmq::i_pollable *zmq::bp_tcp_engine_t::cast_to_pollable ()
{
    return this;
}

void zmq::bp_tcp_engine_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

void zmq::bp_tcp_engine_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Initialise the poll handle.
    handle = poller->add_fd (socket->get_fd (), this);

    if (state == engine_connecting)
        //  Wait for completion of connect() call.
        poller->set_pollout (handle);
    else
        //  Start receiving 'backend protocol' messages.
        poller->set_pollin (handle);
}

void zmq::bp_tcp_engine_t::in_event ()
{
    //  This variable determines whether processing incoming messages is
    //  stuck because of exceeded pipe limits.
    bool stuck = read_pos < read_size;

    //  If there's no data to process in the buffer, read new data.
    if (read_pos == read_size) {

        //  Read as much data as possible to the read buffer.
        read_size = socket->read (readbuf, readbuf_size);
        read_pos = 0;

        //  Check whether the peer has closed the connection.
        if (read_size == -1) {
            error ();
            return;
        }
    }

    //  If there's at least one unprocessed byte in the buffer, process it.
    if (read_pos < read_size) {

        //  Push the data to the decoder and adjust read position in the buffer.
        int  nbytes = decoder.write (readbuf + read_pos, read_size - read_pos);
        read_pos += nbytes;

         //  If processing was stuck and become unstuck start reading
         //  from the socket. If it was unstuck and became stuck, stop polling
         //  for new data.
         if (stuck) {
             if (read_pos == read_size)

                 //  TODO: Speculative read should be used at this place to
                 //  avoid excessive poll. However, it looks like this can
                 //  result in infinite cycle in some cases, virtually
                 //  preventing other engines' access to CPU. Fix it.
                 poller->set_pollin (handle);
         }
         else {
             if (read_pos < read_size) {
                 poller->reset_pollin (handle);
             }    
         }

        //  If at least one byte was processed, flush any messages decoder
        //  may have produced.
        if (nbytes > 0)
            demux.flush ();
    }
}

void zmq::bp_tcp_engine_t::out_event ()
{
    if (state == engine_connecting) {

        if (socket->socket_error ()) {
            error ();
            return;
        }

//        if (mux.empty ())
//            poller->reset_pollout (handle);
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
        int nbytes = socket->write (writebuf + write_pos,
            write_size - write_pos);

        //  Handle problems with the connection.
        if (nbytes == -1) {
            error ();
            return;
        }

        write_pos += nbytes;
    }
}

void zmq::bp_tcp_engine_t::unregister_event ()
{
    //  TODO: Implement full-blown shut-down mechanism.
    //  For now, we'll just close the underlying socket.
    if (state != engine_shutting_down) {
        poller->rm_fd (handle); 
        socket->close ();
    }
}

void zmq::bp_tcp_engine_t::revive (pipe_t *pipe_)
{
    //  Mark pipe as alive.
    engine_base_t <true,true>::revive (pipe_);

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

void zmq::bp_tcp_engine_t::head (pipe_t *pipe_, int64_t position_)
{
    engine_base_t <true,true>::head (pipe_, position_);

    //  This command may have unblocked the pipe - start receiving messages.
    in_event ();
}

void zmq::bp_tcp_engine_t::send_to (pipe_t *pipe_)
{
    //  If pipe limits are set, POLLIN may be turned off
    //  because there are no pipes to send messages to.
    //  So, if this is the first pipe in demux, start polling.
    if (state == engine_connected && demux.no_pipes ())
        poller->set_pollin (handle);    

    engine_base_t <true,true>::send_to (pipe_);
}

void zmq::bp_tcp_engine_t::receive_from (pipe_t *pipe_)
{
    engine_base_t <true,true>::receive_from (pipe_);

    //  If we are already in shut down phase, initiate shut down of the pipe
    //  immediately.
    if (state == engine_shutting_down)
        pipe_->terminate_reader ();
    
    if (state == engine_connected)
        poller->set_pollout (handle);
}

void zmq::bp_tcp_engine_t::terminate_pipe (pipe_t *pipe_)
{
    engine_base_t <true, true>::terminate_pipe (pipe_);

    //  If there is no pipe in the mux and demux and we have processed
    //  send_to or recv_from command delete engine object.
    if (mux.dead () && demux.dead ()) {
        poller->rm_engine (this);
        delete this;
    }
}

void zmq::bp_tcp_engine_t::terminate_pipe_ack (pipe_t *pipe_)
{
    engine_base_t <true, true>::terminate_pipe_ack (pipe_);

    //  If there is no pipe in the mux and demux and we have processed
    //  send_to or recv_from command delete engine object.
    if (mux.dead () && demux.dead ()) {
        poller->rm_engine (this);
        delete this;
    }
}
