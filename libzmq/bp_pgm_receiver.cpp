/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <zmq/bp_pgm_receiver.hpp>

//#define PGM_RECEIVER_DEBUG
//#define PGM_RECEIVER_DEBUG_LEVEL 0

// level 1 = key behaviour
// level 2 = processing flow
// level 4 = infos

#ifndef PGM_RECEIVER_DEBUG
#   define zmq_log(n, ...)  while (0)
#else
#   define zmq_log(n, ...)    do { if ((n) <= PGM_RECEIVER_DEBUG_LEVEL) { printf (__VA_ARGS__);}} while (0)
#endif

zmq::bp_pgm_receiver_t *zmq::bp_pgm_receiver_t::create (
    i_thread *calling_thread_, i_thread *thread_, const char *network_, 
    const char *local_object_, size_t readbuf_size_)
{
    bp_pgm_receiver_t *instance = new bp_pgm_receiver_t (calling_thread_,
        thread_, network_, local_object_, readbuf_size_);
    assert (instance);

    return instance;
}

zmq::bp_pgm_receiver_t::bp_pgm_receiver_t (i_thread *calling_thread_, 
    i_thread *thread_, const char *network_, const char *local_object_, 
    size_t readbuf_size_) :
    decoder (&demux),
    epgm_socket (true, false, network_, readbuf_size_),
    iov (NULL), iov_len (0)
{
    //  Calculate number of iovecs to fill the entire buffer.
    //  One iovec represents one apdu.
    iov_len = epgm_socket.get_max_apdu_at_once (readbuf_size_);
    iov = new iovec [iov_len];

    //  Register PGM engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_pgm_receiver_t::~bp_pgm_receiver_t ()
{
    if (iov) {
        delete [] iov;
    }
}

zmq::engine_type_t zmq::bp_pgm_receiver_t::type ()
{
    return engine_type_fd;
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

    //  Allocate 2 fds one for socket second for waiting pipe
    int socket_fd;
    int waiting_pipe_fd;

    //  Fill socket_fd and waiting_pipe_fd from PGM transport
    int nfds = epgm_socket.get_receiver_fds (&socket_fd, &waiting_pipe_fd);

    assert (nfds == 2);

    //  Add socket_fd into poller.
    socket_handle = poller->add_fd (socket_fd, this);

    //  Add waiting_pipe_fd into poller
    pipe_handle = poller->add_fd (waiting_pipe_fd, this);

    //  Set POLLIN for both handlers
    poller->set_pollin (pipe_handle);
    poller->set_pollin (socket_handle);
}

void zmq::bp_pgm_receiver_t::in_event ()
{
    // POLLIN event from socket or waiting_pipe
    size_t nbytes = epgm_socket.read_pkt_with_offset (iov, iov_len);

    zmq_log (2, "received %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

    // No data received
    if (!nbytes) {
        return;
    }

    iovec *iov_to_write = iov;
    iovec *iov_to_write_end = iov_to_write + iov_len;

    //  Push the data to the decoder
    while (nbytes) {

        assert (nbytes > 0);
        assert (iov_to_write <= iov_to_write_end);

        zmq_log (2, "writting %iB into decoder, %s(%i)\n", (int)iov_to_write->iov_len, 
            __FILE__, __LINE__);
        decoder.write ((unsigned char*)iov_to_write->iov_base, iov_to_write->iov_len);

        nbytes -= iov_to_write->iov_len;
        iov_to_write++;

    }

    //  Flush any messages decoder may have produced to the dispatcher
    demux.flush ();
}

void zmq::bp_pgm_receiver_t::out_event ()//pollfd *pfd_, int count_, int index_)
{
    assert (false);
}

//void zmq::bp_pgm_engine_t::error_event ()
//{
//    assert (false);
//}

void zmq::bp_pgm_receiver_t::unregister_event ()
{
    assert (false);
}

void zmq::bp_pgm_receiver_t::process_command 
    (const engine_command_t &command_)
{
    switch (command_.type) {
        case engine_command_t::send_to:
            demux.send_to (command_.args.receive_from.pipe);
            poller->set_pollin (socket_handle);
            poller->set_pollin (pipe_handle);
            break;
        default:
            assert (false);
    }
}

