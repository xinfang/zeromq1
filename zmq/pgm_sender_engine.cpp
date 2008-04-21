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

zmq::pgm_sender_engine_t::pgm_sender_engine_t (dispatcher_t *dispatcher_, int engine_id_,
      const char *network_, uint16_t port_, int source_engine_id_) :
    proxy (dispatcher_, engine_id_),
    encoder (&proxy, source_engine_id_),
    epgm_socket (false, false, network_, port_),
//    writebuf_size (8192),
    write_size (0),
    write_pos (0)

{
//    writebuf = new unsigned char [writebuf_size];
//    assert (writebuf);
}

zmq::pgm_sender_engine_t::~pgm_sender_engine_t ()
{
//    delete [] writebuf;
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

                printf ("received %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

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
                // get memory slice from tx window
                txw_slice = epgm_socket.alloc_one (&txw_max_tsdu);
                write_size = encoder.read (txw_slice + sizeof (uint16_t), txw_max_tsdu - sizeof (uint16_t));
                write_pos = 0;

                printf ("read %iB from encoder, %s(%i)\n", (int)write_size, __FILE__, __LINE__);

                //  If there are no data to write stop polling for output
                if (!write_size) {
                    pfd_ [index_].events ^= POLLOUT;
                    printf ("POLLOUT stopped, %s(%i)\n", __FILE__, __LINE__);
                } else {
                    // Addning uint16_t for offset in a case when encoder returned > 0B
                    write_size += sizeof (uint16_t);
                }
            }

            //  If there are any data to write in write buffer, write as much as
            //  possible to the socket.
            if (write_pos < write_size) {
                size_t nbytes = epgm_socket.write_pkt (txw_slice + write_pos,
                    write_size - write_pos, 0);

                printf ("wrote %iB/%iB, %s(%i)\n", (int)(write_size - write_pos), (int)nbytes, __FILE__, __LINE__);
                assert (write_size - write_pos >= nbytes);

                write_pos += nbytes;
            }
            break;
        default:
            assert (0);
    }
}
