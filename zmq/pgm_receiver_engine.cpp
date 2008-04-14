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

#include "pgm_receiver_engine.hpp"

zmq::pgm_receiver_engine_t::pgm_receiver_engine_t (dispatcher_t *dispatcher_, int engine_id_,
      const char *network_, uint16_t port_,
      /*int source_engine_id_,*/ int destination_engine_id_//,
      /*size_t writebuf_size_, size_t readbuf_size_*/) :
    proxy (dispatcher_, engine_id_),
//    encoder (&proxy, source_engine_id_),
    decoder (&proxy, destination_engine_id_),
    pgm_receiver (network_, port_),
    events (POLLIN)
/*    writebuf_size (writebuf_size_),
    readbuf_size (readbuf_size_),
    write_size (0),
    write_pos (0)
*/
{
/*    writebuf = (unsigned char*) malloc (writebuf_size);
    assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    assert (readbuf);
*/
}

zmq::pgm_receiver_engine_t::~pgm_receiver_engine_t ()
{
//    free (readbuf);
//    free (writebuf);
}

void zmq::pgm_receiver_engine_t::set_signaler (i_signaler *signaler_)
{
    proxy.set_signaler (signaler_);
}

int zmq::pgm_receiver_engine_t::get_fd_count ()
{
    int nfds = pgm_receiver.get_fd_count ();
    assert (nfds == pgm_receiver_fds);

    return nfds;
}

int zmq::pgm_receiver_engine_t::get_pfds (pollfd *pfd_, int count_)
{
    return pgm_receiver.get_pfds (pfd_, count_);
}

void zmq::pgm_receiver_engine_t::revive (pollfd *pfd_, int count_, int engine_id_)
{
    assert (0);
    //  There is at least one engine that has messages ready - start polling
    //  the socket for writing.
//    proxy.revive (engine_id_);
//    events |= POLLOUT;
}

void zmq::pgm_receiver_engine_t::in_event (pollfd *pfd_, int count_, int index_)
{
    assert (count_ == pgm_receiver_fds);

    switch (index_) {
        case 0:
            // POLLIN event from recv socket
            {
                iovec *iovs;
    
                size_t nbytes = pgm_receiver.read_msg (&iovs);

                printf ("received %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

                if (!nbytes) {
                    return;
                }

                //  Push the data to the decoder
                while (nbytes > 0) {
                    printf ("writting %iB into decoder, %s(%i)\n", (int)iovs->iov_len, 
                        __FILE__, __LINE__);
                    decoder.write ((unsigned char*)iovs->iov_base, iovs->iov_len);
                    nbytes -= iovs->iov_len;
                    iovs++;
                }

                //  Flush any messages decoder may have produced to the dispatcher
                proxy.flush ();
            }
            break;
        case 1:
            // POLLIN from waitting socket
            assert (0);
            break;
    }
}

void zmq::pgm_receiver_engine_t::out_event (pollfd *pfd_, int count_, int index_)
{
    assert (0);
}
