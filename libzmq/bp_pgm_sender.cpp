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

#include <zmq/bp_pgm_sender.hpp>
#include <zmq/config.hpp>
#include <zmq/formatting.hpp>
#include <zmq/ip.hpp>
#include <iostream>

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

zmq::bp_pgm_sender_t::bp_pgm_sender_t (i_thread *calling_thread_,
      i_thread *thread_, const char *interface_, i_thread *peer_thread_, 
      i_engine *peer_engine_) :
    shutting_down (false),
    encoder (&mux),
    epgm_socket (false, interface_),
    txw_slice (NULL),
    max_tsdu_size (0),
    write_size (0),
    write_pos (0), 
    first_message_offset (-1)

{
    //  Store interface. Note that interface name is not stored in locator.
    char *delim = strchr (interface_, ';');
    assert (delim);

    delim++;

    //  If we are using UDP encapsulation "bp/pgm://udp:mcast_address:port" is
    //  registered into zmq_server.
    if (strlen (interface_) > 4 && interface_ [0] == 'u' && 
          interface_ [1] == 'd' && interface_ [2] == 'p' && 
          interface_ [3] == ':') {
        sprintf (arguments, "bp/pgm://udp:%s", delim);
    } else {
        sprintf (arguments, "bp/pgm://%s", delim);
    }

    // Get max tsdu size from transmit window, 
    // will be used as max size for filling buffer by encoder.
    max_tsdu_size = epgm_socket.get_max_tsdu_size ();

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);

    //  The newly created engine serves as a local destination of messages
    //  I.e. it sends messages received from the peer engine to the socket.
    i_engine *destination_engine = this;

    //  Create the pipe to the newly created engine.
    pipe_t *pipe = new pipe_t (peer_thread_, peer_engine_,
        thread_, destination_engine);
    assert (pipe);

    //  Bind new engine to the destination end of the pipe.
    command_t cmd_receive_from;
    cmd_receive_from.init_engine_receive_from (
        destination_engine, pipe);
    calling_thread_->send_command (thread_, cmd_receive_from);

    //  Bind the peer to the source end of the pipe.
    command_t cmd_send_to;
    cmd_send_to.init_engine_send_to (peer_engine_, pipe);
    calling_thread_->send_command (peer_thread_, cmd_send_to);
}

zmq::bp_pgm_sender_t::~bp_pgm_sender_t ()
{
    if (txw_slice) {
        epgm_socket.free_one_pkt (txw_slice);
    }
}

zmq::i_pollable *zmq::bp_pgm_sender_t::cast_to_pollable ()
{
    return this;
}

void zmq::bp_pgm_sender_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

void zmq::bp_pgm_sender_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Alocate 2 fds for PGM socket.
    int downlink_socket_fd;
    int uplink_socket_fd;

    //  Fill socket_fd from PGM transport.
    epgm_socket.get_sender_fds (&downlink_socket_fd, &uplink_socket_fd);

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

//  In event on sender side means NAK receiving from some peer.
void zmq::bp_pgm_sender_t::in_event ()
{
    epgm_socket.process_NAK ();
}

void zmq::bp_pgm_sender_t::out_event ()
{

    //  POLLOUT event from send socket. If write buffer is empty, 
    //  try to read new data from the encoder.
    if (write_pos == write_size) {

        //  Get memory slice from tx window if we do not have already one.
        if (!txw_slice) {
            txw_slice = epgm_socket.alloc_one_pkt ();
        }

        //  First two bytes /sizeof (uint16_t)/ are used to store message 
        //  offset in following steps.
        write_size = encoder.read (txw_slice + sizeof (uint16_t), 
            max_tsdu_size - sizeof (uint16_t), &first_message_offset);
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
        size_t nbytes = epgm_socket.write_one_pkt_with_offset (txw_slice + 
            write_pos, write_size - write_pos, (uint16_t) first_message_offset);

        //  We can write all data or 0 which means rate limit reached.
        if (write_size - write_pos != nbytes && nbytes != 0) {
            zmq_log (1, "write_size - write_pos %i, nbytes %i, %s(%i)",
                (int)(write_size - write_pos), (int)nbytes, __FILE__, __LINE__);
            assert (false);
        }

        //  PGM rate limit reached nbytes is 0.
        if (!nbytes) {
            zmq_log (1, "pgm rate limit reached, %s(%i)\n", __FILE__, __LINE__);
        }

        //  After sending data slice is owned by tx window.
        if (nbytes) {
            txw_slice = NULL;
        }

        write_pos += nbytes;
    }
}

void zmq::bp_pgm_sender_t::unregister_event ()
{
    // TODO: Implement this. For now we just ignore the event.
}

void zmq::bp_pgm_sender_t::receive_from (pipe_t *pipe_)
{    
    engine_base_t <false, true>::receive_from (pipe_);

    if (shutting_down)
        pipe_->terminate_reader ();
    else
        poller->set_pollout (handle);
}

void zmq::bp_pgm_sender_t::revive (pipe_t *pipe_)
{
    //  We have some messages in encoder.
    if (!shutting_down) {
                
        //  Forward the revive command to the pipe. 
        engine_base_t <false, true>::revive (pipe_);

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

#endif
