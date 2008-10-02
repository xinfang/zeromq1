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

#include <poll.h>

#include "pgm_sender_engine.hpp"

//#define PGM_SENDER_DEBUG
//#define PGM_SENDER_DEBUG_LEVEL 1

// level 1 = key behaviour
// level 2 = processing flow
// level 4 = infos

#ifndef PGM_SENDER_DEBUG
#   define zmq_log(n, ...)  while (0)
#else
#   define zmq_log(n, ...)    do { if ((n) <= PGM_SENDER_DEBUG_LEVEL) { printf (__VA_ARGS__);}} while (0)
#endif


zmq::pgm_sender_engine_t::pgm_sender_engine_t (dispatcher_t *dispatcher_, int engine_id_,
      const char *network_, uint16_t port_, int source_engine_id_) :
    proxy (dispatcher_, engine_id_),
    encoder (&proxy, source_engine_id_),
    epgm_socket (false, false, network_, port_),
    txw_slice (NULL),
    max_tsdu (0),
    write_size (0),
    write_pos (0), 
    first_message_offest (-1)
{
    // Get max tsdu size from transmit window, 
    // will be used as max size for filling buffer by encoder
    max_tsdu = epgm_socket.get_max_tsdu (false);
}

zmq::pgm_sender_engine_t::~pgm_sender_engine_t ()
{
    if (txw_slice) {
        zmq_log (2, "Freeing unused slice\n");
        epgm_socket.free_one_pkt (txw_slice, false);
    }
}

void zmq::pgm_sender_engine_t::set_signaler (i_signaler *signaler_)
{
    proxy.set_signaler (signaler_);
}

int zmq::pgm_sender_engine_t::get_fd_count ()
{
    int nfds = epgm_socket.get_fd_count (EPOLLIN | EPOLLOUT);
    assert (nfds == pgm_sender_fds);

    return nfds;
}

int zmq::pgm_sender_engine_t::get_pfds (pollfd *pfd_, int count_)
{
    return epgm_socket.get_pfds (pfd_, count_, EPOLLIN | EPOLLOUT);
}

void zmq::pgm_sender_engine_t::revive (pollfd *pfd_, int count_, int engine_id_)
{
    assert (count_ == pgm_sender_fds);

    //  There is at least one engine that has messages ready - start polling
    //  the socket for writing.
    proxy.revive (engine_id_);

    assert (count_ == pgm_sender_fds);

    pfd_[2].events |= POLLOUT;
}

void zmq::pgm_sender_engine_t::in_event (pollfd *pfd_, int count_, int index_)
{
    assert (count_ == pgm_sender_fds);

    switch (index_) {
        case pgm_recv_fd_idx:
            // POLLIN event from recv socket 
            // In sender engine it means NAK receiving
            {
                iovec *iovs;
    
                size_t nbytes = 0;//epgm_socket.read_msg (&iovs);
                // debug only 
                pfd_ [index_].events ^= POLLIN;

                zmq_log (2, "received %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

                assert (!nbytes);
            }
            break;
        default:
            assert (0);
    }

}

void zmq::pgm_sender_engine_t::out_event (pollfd *pfd_, int count_, int index_)
{
    assert (count_ == pgm_sender_fds);

    switch (index_) {
        case pgm_send_fd_idx:
            // POLLOUT event from send socket

            //  If write buffer is empty, try to read new data from the encoder
            if (write_pos == write_size) {
                // get memory slice from tx window if we do not have already one
                if (!txw_slice) {
                    txw_slice = epgm_socket.alloc_one_pkt (false);
                    zmq_log (2, "Alocated packet in tx window\n");
                }

                write_size = encoder.read (txw_slice + sizeof (uint16_t), 
                    max_tsdu - sizeof (uint16_t), &first_message_offest);
                write_pos = 0;

                zmq_log (2, "read %iB from encoder offset %i, %s(%i)\n", 
                    (int)write_size, (int)first_message_offest, __FILE__, __LINE__);

                //  If there are no data to write stop polling for output
                if (!write_size) {
                    pfd_ [index_].events ^= POLLOUT;
                    zmq_log (2, "POLLOUT stopped, %s(%i)\n", __FILE__, __LINE__);
                } else {
                    // Addning uint16_t for offset in a case when encoder returned > 0B
                    write_size += sizeof (uint16_t);
                }
            }

            //  If there are any data to write in write buffer, write them into the socket
            //  note that all data has to written in one write
            if (write_pos < write_size) {
                size_t nbytes = epgm_socket.write_one_pkt_with_offset (txw_slice + write_pos,
                    write_size - write_pos, first_message_offest);

                zmq_log (2, "wrote %iB/%iB, %s(%i)\n", (int)(write_size - write_pos), (int)nbytes, __FILE__, __LINE__);
               
                // we can write all packer or 0 which means rate limit reached
                if (write_size - write_pos != nbytes && nbytes != 0) {
                    zmq_log (1, "write_size - write_pos %i, nbytes %i, %s(%i)",
                        (int)(write_size - write_pos), (int)nbytes, __FILE__, __LINE__);
                    assert (0);
                }

                if (!nbytes) {
                    zmq_log (1, "pgm rate limit reached, %s(%i)\n", __FILE__, __LINE__);
                }

                // after sending data slice is owned by tx window
                if (nbytes) {
                    txw_slice = NULL;
                }

                write_pos += nbytes;
            }
            break;
        default:
            assert (0);
    }
}
