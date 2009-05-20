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

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <iostream>

#include <zmq/bp_pgm_sender.hpp>
#include <zmq/config.hpp>
#include <zmq/formatting.hpp>
#include <zmq/ip.hpp>
#include <zmq/wire.hpp>
#include <zmq/err.hpp>

//#define PGM_SENDER_DEBUG
//#define PGM_SENDER_DEBUG_LEVEL 4

// level 1 = key behaviour
// level 2 = processing flow
// level 4 = infos

#ifndef PGM_SENDER_DEBUG
#   define zmq_log(n, ...)  while (0)
#else
#   define zmq_log(n, ...)    do { if ((n) <= PGM_SENDER_DEBUG_LEVEL) \
        { printf (__VA_ARGS__);}} while (0)
#endif

zmq::bp_pgm_sender_t::bp_pgm_sender_t (mux_t *mux_, i_thread *calling_thread_,
      i_thread *thread_, const char *interface_, i_thread *peer_thread_, 
      i_engine *peer_engine_) :
    mux (mux_),
    shutting_down (false),
    encoder (mux_),
    pgm_socket (false, interface_),
    out_buffer (NULL),
    out_buffer_size (0),
    write_size (0),
    write_pos (0), 
    first_message_offset (-1)
{
    zmq_assert (mux_);

    //  Store interface. Note that interface name is not stored in locator.
    char *delim = strchr (interface_, ';');
    zmq_assert (delim);

    delim++;

    //  If we are using UDP encapsulation "zmq.pgm://udp:mcast_address:port" is
    //  registered into zmq_server.
    if (strlen (interface_) > 4 && interface_ [0] == 'u' && 
          interface_ [1] == 'd' && interface_ [2] == 'p' && 
          interface_ [3] == ':') {
        sprintf (arguments, "zmq.pgm://udp:%s", delim);
    } else {
        sprintf (arguments, "zmq.pgm://%s", delim);
    }

    //  The newly created engine serves as a local destination of messages
    //  I.e. it sends messages received from the peer engine to the socket.

    //  Create the pipe to the newly created engine.
    i_demux *demux = peer_engine_->get_demux ();
    pipe_t *pipe = new pipe_t (peer_thread_, demux,
        thread_, mux, mux->get_swap_size ());
    zmq_assert (pipe);

    //  Bind new engine to the destination end of the pipe.
    command_t mux_cmd;
    mux_cmd.init_attach_pipe_to_mux (mux, pipe);
    calling_thread_->send_command (thread_, mux_cmd);

    //  Bind the peer to the source end of the pipe.
    command_t demux_cmd;
    demux_cmd.init_attach_pipe_to_demux (demux, pipe);
    calling_thread_->send_command (peer_thread_, demux_cmd);
}

zmq::bp_pgm_sender_t::~bp_pgm_sender_t ()
{
    if (out_buffer) {
        pgm_socket.free_buffer (out_buffer);
    }
}

void zmq::bp_pgm_sender_t::start (i_thread *current_thread_,
    i_thread *engine_thread_)
{
    mux->register_engine (this);

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_pollable (this);
    current_thread_->send_command (engine_thread_, command);
}

zmq::i_demux *zmq::bp_pgm_sender_t::get_demux ()
{
    zmq_assert (false);
    return NULL;
}

zmq::i_mux *zmq::bp_pgm_sender_t::get_mux ()
{
    return mux;
}

void zmq::bp_pgm_sender_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Alocate 2 fds for PGM socket.
    int downlink_socket_fd;
    int uplink_socket_fd;

    //  Fill socket_fd from PGM transport.
    pgm_socket.get_sender_fds (&downlink_socket_fd, &uplink_socket_fd);

    //  Add downlink_socket_fd into poller.
    handle = poller->add_fd (downlink_socket_fd, this);

    //  Set POLLOUT for downlink_socket_handle.
    poller->set_pollout (handle);

    //  Add uplink_socket_fd into the poller.
    handle_t uplink_handle = poller->add_fd (uplink_socket_fd, this);

    //  Set POLLIN. We wont never want to stop polling for uplink = we never
    //  want to stop porocess NAKs.
    poller->set_pollin (uplink_handle);
}

//  In event on sender side means NAK or SPMR receiving from some peer.
void zmq::bp_pgm_sender_t::in_event ()
{
    pgm_socket.process_upstream ();
}

void zmq::bp_pgm_sender_t::out_event ()
{

    //  POLLOUT event from send socket. If write buffer is empty, 
    //  try to read new data from the encoder.
    if (write_pos == write_size) {

        //  Get buffer if we do not have already one.
        if (!out_buffer) {
            out_buffer = (unsigned char*) 
                pgm_socket.get_buffer (&out_buffer_size);
        }

        zmq_assert (out_buffer_size > 0);

        //  First two bytes /sizeof (uint16_t)/ are used to store message 
        //  offset in following steps.
        write_size = encoder.read (out_buffer + sizeof (uint16_t), 
            out_buffer_size - sizeof (uint16_t), &first_message_offset);
        write_pos = 0;

        //  If there are no data to write stop polling for output.
        if (!write_size) {
            poller->reset_pollout (handle);
        } else {
            // Addning uint16_t for offset in a case when encoder returned > 0B.
            write_size += sizeof (uint16_t);
        }
    }

    //  If there are any data to write, write them into the socket.
    //  Note that all data has to written in one write_one_pkt_with_offset call.
    if (write_pos < write_size) {
        size_t nbytes = write_one_pkt_with_offset (out_buffer + write_pos, 
            write_size - write_pos, (uint16_t) first_message_offset);

        //  We can write all data or 0 which means rate limit reached.
        if (write_size - write_pos != nbytes && nbytes != 0) {
            zmq_log (1, "write_size - write_pos %i, nbytes %i, %s(%i)",
                (int)(write_size - write_pos), (int)nbytes, __FILE__, __LINE__);
            zmq_assert (false);
        }

        //  PGM rate limit reached nbytes is 0.
        if (!nbytes) {
            zmq_log (1, "pgm rate limit reached, %s(%i)\n", __FILE__, __LINE__);
        }

        //  After sending data slice is owned by tx window.
        if (nbytes) {
            out_buffer = NULL;
        }

        write_pos += nbytes;
    }
}

void zmq::bp_pgm_sender_t::timer_event ()
{
    //  We are setting no timers. We shouldn't get this event.
    zmq_assert (false);
}

void zmq::bp_pgm_sender_t::unregister_event ()
{
    // TODO: Implement this. For now we just ignore the event.
}

void zmq::bp_pgm_sender_t::receive_from ()
{
    if (!shutting_down)
        poller->set_pollout (handle);
}

void zmq::bp_pgm_sender_t::revive ()
{
    //  We have some messages in encoder.
    if (!shutting_down) {

        //  There is at least one engine (that one which sent revive) that 
        //  has messages ready. Try to write data to the socket, thus 
        //  eliminating one polling for POLLOUT event.
        //  Note that if write_size is zero it means that buffer is empty and
        //  we can read data from encoder.
        if (!write_size) {
            poller->set_pollout (handle);
            out_event ();
        }
    }
}

const char *zmq::bp_pgm_sender_t::get_arguments ()
{
    return arguments;
}

void zmq::bp_pgm_sender_t::head ()
{
    zmq_assert (false);
}

void zmq::bp_pgm_sender_t::send_to ()
{
    zmq_assert (false);
}

size_t zmq::bp_pgm_sender_t::write_one_pkt_with_offset (unsigned char *data_, 
    size_t size_, uint16_t offset_)
{

    //  Put offset information in the buffer.
    put_uint16 (data_, offset_);
   
    //  Send data.
    size_t nbytes = pgm_socket.send (data_, size_);

    return nbytes;
}

#endif
