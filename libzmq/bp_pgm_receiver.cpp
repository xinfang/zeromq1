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

#include <zmq/bp_pgm_receiver.hpp>
#include <zmq/wire.hpp>
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
      i_thread *thread_, const char *network_, size_t readbuf_size_, 
      const char *arguments_) :
    joined (false),
    shutting_down (false),
    decoder (demux),
    pgm_socket (NULL)
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

        //  Shift network_ pointer beyond ':'.
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
    pgm_socket = new pgm_socket_t (true, network.c_str (), readbuf_size_);

    //  Register PGM engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_pgm_receiver_t::~bp_pgm_receiver_t ()
{
    //  Cleanup.
    delete pgm_socket;
}

zmq::i_pollable *zmq::bp_pgm_receiver_t::cast_to_pollable ()
{
    return this;
}

void zmq::bp_pgm_receiver_t::get_watermarks (int64_t *hwm_, int64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

uint64_t zmq::bp_pgm_receiver_t::get_swap_size ()
{
    return 0;
}

void zmq::bp_pgm_receiver_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Allocate 2 fds one for socket second for waiting pipe.
    int socket_fd;
    int waiting_pipe_fd;

    //  Fill socket_fd and waiting_pipe_fd from PGM transport
    pgm_socket->get_receiver_fds (&socket_fd, &waiting_pipe_fd);

    //  Add socket_fd into poller.
    socket_handle = poller->add_fd (socket_fd, this);

    //  Add waiting_pipe_fd into poller.
    pipe_handle = poller->add_fd (waiting_pipe_fd, this);

    //  Set POLLIN for both handlers.
    poller->set_pollin (pipe_handle);
    poller->set_pollin (socket_handle);
}

void zmq::bp_pgm_receiver_t::reconnect (void)
{
    int socket_fd;
    int waiting_pipe_fd;

    //  Remove old fds from poller.
    poller->rm_fd (socket_handle);
    poller->rm_fd (pipe_handle);

    //  Close / open PGM transport.
    pgm_socket->close_transport ();
    pgm_socket->open_transport ();

    //  Fill socket_fd and waiting_pipe_fd from new PGM transport
    pgm_socket->get_receiver_fds (&socket_fd, &waiting_pipe_fd);

    //  Add socket_fd into poller.
    socket_handle = poller->add_fd (socket_fd, this);

    //  Add waiting_pipe_fd into poller.
    pipe_handle = poller->add_fd (waiting_pipe_fd, this);

    //  Set POLLIN for both handlers.
    poller->set_pollin (pipe_handle);
    poller->set_pollin (socket_handle);
}

//  POLLIN event from socket or waiting_pipe.
void zmq::bp_pgm_receiver_t::in_event ()
{

    void *data_with_offset;
    ssize_t nbytes = 0;

    //  Read all data from pgm socket.
    while ((nbytes = receive_with_offset (&data_with_offset)) > 0) {

        //  Push all the data to the decoder.
        decoder.write ((unsigned char*)data_with_offset, nbytes);
    }

    //  Flush any messages decoder may have produced to the dispatcher.
    demux->flush ();

    //  Data loss detected.
    if (nbytes == -1) {

        //  Throw message in progress from decoder
        decoder.reset ();

        //  Insert "gap" message into the pipes.
        demux->gap ();

        //  PGM receive is not joined anymore.
        joined = false;
        
        //  Recreate PGM transport.
        reconnect ();
    }
}

void zmq::bp_pgm_receiver_t::out_event ()
{
    assert (false);
}

void zmq::bp_pgm_receiver_t::timer_event ()
{
    //  We are setting no timers. We shouldn't get this event.
    assert (false);
}

void zmq::bp_pgm_receiver_t::unregister_event ()
{
    // TODO: Implement this. For now we just ignore the event.
}

void zmq::bp_pgm_receiver_t::send_to (pipe_t *pipe_)
{
    //  If pipe limits are set, POLLIN may be turned off
    //  because there are no pipes to send messages to.
    //  So, if this is the first pipe in demux, start polling.
    if (!shutting_down && demux->no_pipes ()) {
        poller->set_pollin (socket_handle);
        poller->set_pollin (pipe_handle);      
    }

    //  Start sending messages to a pipe.
    engine_base_t <true, false>::send_to (pipe_);
}

ssize_t zmq::bp_pgm_receiver_t::receive_with_offset 
    (void **data_)
{

    //  Data from PGM socket.
    void *rd = NULL;
    unsigned char *raw_data = NULL;

    // Read data from underlying pgm_socket.
    ssize_t nbytes = pgm_socket->receive ((void**) &rd);
    raw_data = (unsigned char*) rd;

    //  No ODATA or RDATA.
    if (!nbytes)
        return 0;

    //  Data loss.
    if (nbytes == -1) {
        return -1;
    }

    // Read offset of the fist message in current APDU.
    uint16_t apdu_offset = get_uint16 (raw_data);

    // Shift raw_data & decrease nbytes by the first message offset 
    // information (sizeof uint16_t).
    *data_ = raw_data +  sizeof (uint16_t);
    nbytes -= sizeof (uint16_t);

    //  There is not beginning of the message in current APDU and we
    //  are not joined jet -> throwing data.
    if (apdu_offset == 0xFFFF && !joined) {
        *data_ = NULL;
        return 0;
    }

    //  Now is the possibility to join the stream.
    if (!joined) {
           
        //  We have to move data to the begining of the first message.
        *data_ = (unsigned char *)*data_ + apdu_offset;
        nbytes -= apdu_offset;

        // Joined the stream.
        joined = true;

        zmq_log (2, "joined into the stream, %s(%i)\n", 
            __FILE__, __LINE__);
    }

    return nbytes;
}

#endif

