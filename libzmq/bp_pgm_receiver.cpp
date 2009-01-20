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

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <zmq/bp_pgm_receiver.hpp>
#include <string>

//#define PGM_RECEIVER_DEBUG
//#define PGM_RECEIVER_DEBUG_LEVEL 0

// level 1 = key behaviour
// level 2 = processing flow
// level 4 = infos

#ifndef PGM_RECEIVER_DEBUG
#   define zmq_log(n, ...)  while (0)
#else
#   define zmq_log(n, ...)    do { if ((n) <= PGM_RECEIVER_DEBUG_LEVEL) \
        { printf (__VA_ARGS__);}} while (0)
#endif

zmq::bp_pgm_receiver_t::bp_pgm_receiver_t (i_thread *calling_thread_, 
      i_thread *thread_, const char *network_, const char *local_object_, 
      size_t readbuf_size_, const char *arguments_) :
    shutting_down (false),
    decoder (&demux),
    epgm_socket (NULL),
    iov (NULL), iov_len (0)
{
    //  If UDP encapsulation is used network_ parameter contain 
    //  "udp:mcast_address:port". Interface name is comming from bind api
    //  argument in arguments_ variable. 
    //  Final format has to be udp:iface_name;mcast_group:port for UDP 
    //  encapsulation and iface;mcast_group:port otherwise.
    bool udp_encapsulation = false;

    if (strlen (network_) > 4 && network_ [0] == 'u' && 
          network_ [1] == 'd' && network_ [2] == 'p' && 
          network_ [3] == ':') {
        udp_encapsulation = true;

        //  Shift network_ pointer after ':'.
        network_ += 4;
    }

    std::string transport_info (network_);
    std::string mcast_interface (arguments_);
    std::string network;

    //  Construct string for epgm_socket creation.
    if (udp_encapsulation) {
        network = "udp:" + mcast_interface + ";" + network_;
    } else {
        network = mcast_interface + ";" + network_;
    }

    //  Create epgm_socket object
    epgm_socket = new epgm_socket_t (true, network.c_str (), readbuf_size_);

    //  Calculate number of iovecs to fill the entire buffer.
    //  One iovec represents one apdu.
    iov_len = epgm_socket->get_max_apdu_at_once (readbuf_size_);
    iov = new iovec [iov_len];

    //  Register PGM engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_pgm_receiver_t::~bp_pgm_receiver_t ()
{

    //  Cleanup.
    delete epgm_socket;

    if (iov) {
        delete [] iov;
    }
}

zmq::i_pollable *zmq::bp_pgm_receiver_t::cast_to_pollable ()
{
    return this;
}

void zmq::bp_pgm_receiver_t::get_watermarks (uint64_t *hwm_, uint64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

void zmq::bp_pgm_receiver_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Allocate 2 fds one for socket second for waiting pipe.
    int socket_fd;
    int waiting_pipe_fd;

    //  Fill socket_fd and waiting_pipe_fd from PGM transport
    epgm_socket->get_receiver_fds (&socket_fd, &waiting_pipe_fd);

    //  Add socket_fd into poller.
    socket_handle = poller->add_fd (socket_fd, this);

    //  Add waiting_pipe_fd into poller.
    pipe_handle = poller->add_fd (waiting_pipe_fd, this);

    //  Set POLLIN for both handlers.
    poller->set_pollin (pipe_handle);
    poller->set_pollin (socket_handle);
}

void zmq::bp_pgm_receiver_t::in_event ()
{
    //  POLLIN event from socket or waiting_pipe.
    size_t nbytes = epgm_socket->read_pkt_with_offset (iov, iov_len);

    //  No ODATA/RDATA received.
    if (!nbytes) {
        return;
    }

    iovec *iov_to_write = iov;
    iovec *iov_to_write_end = iov_to_write + iov_len;

    //  Push all the data to the decoder.
    while (nbytes) {

        assert (nbytes > 0);
        assert (iov_to_write <= iov_to_write_end);

        decoder.write ((unsigned char*)iov_to_write->iov_base, 
            iov_to_write->iov_len);

        nbytes -= iov_to_write->iov_len;
        iov_to_write++;

    }

    //  Flush any messages decoder may have produced to the dispatcher.
    demux.flush ();
}

void zmq::bp_pgm_receiver_t::out_event ()
{
    assert (false);
}

void zmq::bp_pgm_receiver_t::unregister_event ()
{
    assert (false);
}

void zmq::bp_pgm_receiver_t::send_to (const char *exchange_, pipe_t *pipe_)
{
    //  If pipe limits are set, POLLIN may be turned off
    //  because there are no pipes to send messages to.
    //  So, if this is the first pipe in demux, start polling.
    if (!shutting_down && demux.no_pipes ()) {
        poller->set_pollin (socket_handle);
        poller->set_pollin (pipe_handle);      
    }

    //  Start sending messages to a pipe.
    demux.send_to (pipe_);
}

#endif

