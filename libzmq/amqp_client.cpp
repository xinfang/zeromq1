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

#if defined ZMQ_HAVE_AMQP

#include <stdio.h>

#include <zmq/amqp_client.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/config.hpp>

zmq::amqp_client_t::amqp_client_t (i_thread *calling_thread_,
      i_thread *thread_, const char *hostname_, const char *local_object_,
      const char *arguments_) :
    state (state_connecting),
    writebuf_size (bp_out_batch_size),
    write_size (0),
    write_pos (0),
    readbuf_size (bp_in_batch_size),
    read_size (0),
    read_pos (0),
    socket (hostname_),
    poller (NULL),
    local_object (local_object_),
    arguments (arguments_)
{
    //  Allocate read and write buffers.
    writebuf = (unsigned char*) malloc (writebuf_size);
    errno_assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    errno_assert (readbuf);

    decoder = new amqp_decoder_t (demux, this);
    errno_assert (decoder);
    encoder = new amqp_encoder_t (&mux, arguments_);
    errno_assert (encoder);

    //  Register AMQP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::amqp_client_t::~amqp_client_t ()
{
    delete encoder;
    delete decoder;

    free (readbuf);
    free (writebuf);
}

zmq::i_pollable *zmq::amqp_client_t::cast_to_pollable ()
{
    return this;
}

void zmq::amqp_client_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    //  TODO: Rename bp_hwm & bp_lwm to generic "connection_hwm" &
    //  "connection_lwm" it is not tied strictly to the backend protocol.
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

int64_t zmq::amqp_client_t::get_swap_size ()
{
    return 0;
}

void zmq::amqp_client_t::revive (pipe_t *pipe_)
{
    if (state != state_connecting && state != state_shutting_down) {

        engine_base_t <true, true>::revive (pipe_);

        //  There is at least one engine that has messages ready. Try to
        //  write data to the socket, thus eliminating one polling
        //  for POLLOUT event.
        if (write_size == 0) {
	    poller->set_pollout (handle);
            out_event ();
        }
    }
}

void zmq::amqp_client_t::head (pipe_t *pipe_, int64_t position_)
{
    //  Forward pipe head position to the appropriate pipe.
    if (state != state_connecting && state != state_shutting_down) {
        engine_base_t <true, true>::head (pipe_, position_);
        in_event ();
    }
}

void zmq::amqp_client_t::send_to (pipe_t *pipe_)
{
    //  If pipe limits are set, POLLIN may be turned off
    //  because there are no pipes to send messages to.
    //  So, if this is the first pipe in demux, start polling.
    if (state != state_shutting_down && demux->no_pipes ())
        poller->set_pollin (handle);

    //  Start sending messages to a pipe.
    engine_base_t <true, true>::send_to (pipe_);
}

void zmq::amqp_client_t::receive_from (pipe_t *pipe_)
{
    //  Start receiving messages from a pipe.
    engine_base_t <true, true>::receive_from (pipe_);

    if (state == state_shutting_down)
        pipe_->terminate_reader ();

    if (state != state_connecting && state != state_shutting_down)
        poller->set_pollout (handle);
}

void zmq::amqp_client_t::register_event (i_poller *poller_)
{
    assert (state == state_connecting);

    //  Store the callback.
    poller = poller_;

    //  Initialise the poll handle.
    handle = poller->add_fd (socket.get_fd (), this);

    //  Wait for completion of connect() call.
    poller->set_pollout (handle);
}

void zmq::amqp_client_t::in_event ()
{
    //  Following code should be invoked when async connect causes POLLERR
    //  rather than POLLOUT.
    if (state == state_connecting) {
        assert (socket.socket_error ());
        error ();
        return;
    }

    //  This variable determines whether processing incoming messages is
    //  stuck because of exceeded pipe limits.
    bool stuck = read_pos < read_size;

    //  If there's no data to process in the buffer, read new data.
    if (read_pos == read_size) {

        //  Read as much data as possible to the read buffer.
        read_size = socket.read (readbuf, readbuf_size);
        read_pos = 0;

        //  The other party closed the connection.
        if (read_size == -1) {
            error ();
            return;
        }
    }

    //  If there's at least one unprocessed byte in the buffer, process it.
    if (read_pos < read_size) {

        //  Push the data to the decoder and adjust read position in the buffer.
        int  nbytes = decoder->decoder_t<amqp_decoder_t>::write (
            readbuf + read_pos, read_size - read_pos);
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
            demux->flush ();
    }
}

void zmq::amqp_client_t::out_event ()
{
    //  Following code is a handler for async connect termination.
    if (state == state_connecting) {

        if (socket.socket_error ()) {
            error ();
            return;
        }

        //  Keep pollout set - there's AMQP protocol header to send.
        //  Start polling for incoming frames as well.
        poller->set_pollin (handle);
        state = state_waiting_for_connection_start;
        return;
    }

    //  If write buffer is empty, try to read new data from the encoder.
    if (write_pos == write_size) {

        write_size = encoder->zmq::encoder_t<amqp_encoder_t>::read (
            writebuf, writebuf_size);
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

        //  The other party closed the connection.
        if (nbytes == -1) {
            error ();
            return;
        }

        write_pos += nbytes;
    }
}

void zmq::amqp_client_t::timer_event ()
{
    assert (state == state_waiting_for_reconnect);
    reconnect ();
}

void zmq::amqp_client_t::unregister_event ()
{
    //  TODO: Implement full-blown shut-down mechanism.
    //  For now, we'll just close the underlying socket.
    if (state != state_waiting_for_reconnect &&
          state != state_shutting_down) {
        poller->rm_fd (handle);
        socket.close ();
    }
}

void zmq::amqp_client_t::connection_start (
    uint16_t channel_,
    uint8_t version_major_,
    uint8_t version_minor_,
    const i_amqp::field_table_t &/* server_properties_ */,
    const i_amqp::longstr_t /* mechanisms_ */,
    const i_amqp::longstr_t /* locales_ */)
{
    assert (channel_ == 0);
    assert (state == state_waiting_for_connection_start);

    //  Check the version info.
    assert (version_major_ == 0);
    assert (version_minor_ == 9);

    //  TODO: Security mechanisms and locales should be checked. Client should
    //  use user-supplied login/password rather than hard-wired guest/guest.
    encoder->connection_start_ok (0, i_amqp::field_table_t (), "PLAIN",
        i_amqp::longstr_t ("\x00guest\x00guest", 12), "en_US");

    state = state_waiting_for_connection_tune;

    //  There are data to send - start polling for output.
    poller->set_pollout (handle);
}

void zmq::amqp_client_t::connection_tune (
    uint16_t channel_,
    uint16_t /* channel_max_ */,
    uint32_t /* frame_max_ */,
    uint16_t /* heartbeat_ */)
{
    assert (channel_ == 0);
    assert (state == state_waiting_for_connection_tune);

    //  TODO: Frame size should be adjusted to match server's min size
    //  TODO: Heartbeats are not implemented at the moment
    encoder->connection_tune_ok (0, 1, i_amqp::frame_min_size, 0);

    //  TODO: Virtual host name should be suplied by client application 
    //  rather than hardwired
    encoder->connection_open (0, "/", "", true);
    
    state = state_waiting_for_connection_open_ok;

    //  There are data to send - start polling for output.
    poller->set_pollout (handle);
}

void zmq::amqp_client_t::connection_open_ok (
    uint16_t channel_,
    const i_amqp::shortstr_t /* reserved_1_ */)
{
    assert (channel_ == 0);
    assert (state == state_waiting_for_connection_open_ok);

    encoder->channel_open (0, "");

    state = state_waiting_for_channel_open_ok;

    //  There are data to send - start polling for output.
    poller->set_pollout (handle);
}

void zmq::amqp_client_t::channel_open_ok (
    uint16_t channel_,
    const i_amqp::longstr_t /* reserved_1_ */)
{
    assert (state == state_waiting_for_channel_open_ok);

    //  Store the ID of the open channel.
    channel = channel_;

    i_amqp::field_table_t queue_args;
    encoder->queue_declare (channel, 0, arguments.c_str (),
        false, false, false, false, false, queue_args);

    state = state_waiting_for_queue_declare_ok;

    //  There are data to send - start polling for output.
    poller->set_pollout (handle);
}

void zmq::amqp_client_t::queue_declare_ok (
    uint16_t channel_,
    const i_amqp::shortstr_t /* queue_ */,
    uint32_t /* message_count_ */,
    uint32_t /* consumer_count_ */)
{
    assert (channel_ == channel);
    assert (state == state_waiting_for_queue_declare_ok);

    i_amqp::field_table_t consume_args;
    encoder->basic_consume (channel, 0, arguments.c_str (), "",
        true, true, false, false, consume_args);

    state = state_waiting_for_basic_consume_ok;

    //  There are data to send - start polling for output.
    poller->set_pollout (handle);
}

void zmq::amqp_client_t::basic_consume_ok (
    uint16_t channel_,
    const i_amqp::shortstr_t /* consumer_tag_ */)
{
    assert (channel_ == channel);
    assert (state == state_waiting_for_basic_consume_ok);

    encoder->flow (true, channel);
    decoder->flow (true, channel);
    state = state_active;

    //  Start polling for out - in case there are messages already prepared
    //  to be sent via this connection.
    poller->set_pollout (handle);   

}

void zmq::amqp_client_t::channel_close (
    uint16_t channel_,
    uint16_t /* reply_code_ */,
    const i_amqp::shortstr_t reply_text_,
    uint16_t /* class_id_ */,
    uint16_t /* method_id_ */)
{
    assert (channel_ == channel);
    printf ("AMQP error received: %s\n", reply_text_.data);
    error ();
}

void zmq::amqp_client_t::connection_close (
    uint16_t channel_,
    uint16_t /* reply_code_ */,
    const i_amqp::shortstr_t reply_text_,
    uint16_t /* class_id_ */,
    uint16_t /* method_id_ */)
{
    assert (channel_ == 0);
    printf ("AMQP error received: %s\n", reply_text_.data);
    error ();
}

void zmq::amqp_client_t::error ()
{
    if (state != state_connecting && state != state_waiting_for_reconnect) {

        //  Push a gap notification to the pipes.
        demux->gap ();

        //  Clean half-processed inbound and outbound data.
        encoder->reset ();
        decoder->reset ();
    }

    //  Report connection failure to the client.
    //  If there is no error handler registered, continue quietly.
    //  If error handler returns true, continue quietly.
    //  If error handler returns false, crash the application.
    error_handler_t *eh = get_error_handler ();
    if (eh && !eh (local_object.c_str ()))
        assert (false);

    //  Reestablish the connection.
    reconnect ();
}

void zmq::amqp_client_t::reconnect ()
{
    if (state != state_waiting_for_reconnect) {

        //  Stop polling the socket.
        poller->rm_fd (handle);

        //  Close the socket.
        socket.close ();

        //  Clear data buffers.
        read_pos = read_size;
        write_pos = write_size;
    }

    //  This is the case when we've tried to reconnect but the attmpt have
    //  failed. We are going to wait a while before trying to reconnect anew
    //  to prevent reconnect consuming 100% of the processor time.
    if (state == state_connecting) {
        poller->add_timer (this);
        state = state_waiting_for_reconnect;
        return;
    }

    //  Reopen the socket. This initiates the TCP connection establishment.
    //  If the reconnection is unsuccessfull wait a while till attempting
    //  it anew.
    socket.reopen (); 
    if (socket.get_fd () == retired_fd) {
        poller->add_timer (this);
        state = state_waiting_for_reconnect;
        return;
    }

    //  The output event is used to signal that we can get
    //  the connection status. Register our interest in it.
    handle = poller->add_fd (socket.get_fd (), this);
    poller->set_pollout (handle);

    state = state_connecting;
}

#endif
