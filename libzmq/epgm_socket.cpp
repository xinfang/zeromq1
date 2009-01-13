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

#include <zmq/platform.hpp>

#if ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <assert.h>

#include <zmq/epgm_socket.hpp>
#include <zmq/wire.hpp>

#define EPGM_SOCKET_DEBUG
#define EPGM_SOCKET_DEBUG_LEVEL 4

// level 1 = key behaviour
// level 2 = processing flow
// level 4 = infos

#ifndef EPGM_SOCKET_DEBUG
#   define zmq_log(n, ...)  while (0)
#else
#   define zmq_log(n, ...)    do { if ((n) <= EPGM_SOCKET_DEBUG_LEVEL) { printf (__VA_ARGS__);}} while (0)
#endif

zmq::epgm_socket_t::epgm_socket_t (bool receiver_, const char *interface_, 
      size_t readbuf_size_) : 
    pgm_socket_t (receiver_, interface_, readbuf_size_),
    joined (false)
{

}

zmq::epgm_socket_t::~epgm_socket_t ()
{

}

size_t zmq::epgm_socket_t::read_pkt_with_offset (iovec *iov_, size_t iov_len_)
{

    // Read data from underlying pgm_socket.
    size_t nbytes = read_pkt (iov_, iov_len_);

    iovec *iov = iov_;
    iovec *iov_last = iov + iov_len_;

    unsigned int translated = 0;

    while (translated < nbytes) {
        
        assert (iov <= iov_last);

        // Read offset of the fist message in current APDU.
        uint16_t apdu_offset = get_uint16 ((unsigned char*)iov->iov_base);

        // Shift iov_base & decrease iov_len of the iovec by sizeof (uint16_t)
        iov->iov_base = (unsigned char*)iov->iov_base + sizeof (uint16_t);
        iov->iov_len -= sizeof (uint16_t);
        nbytes -= sizeof (uint16_t);

        zmq_log (2, "read apdu_offset %i, %s(%i)\n", apdu_offset,
            __FILE__, __LINE__);

        //  There is not beginning of the message in current APDU.
        if (apdu_offset == 0xFFFF) {

            //  If we already joined message stream send all data up.
            if (joined) {
                translated += iov->iov_len;
            } else {
                //  Throwing currnet APDU by setting iov_len to 0 that 
                //  iov_base is not pushed into encoder.
                nbytes -= iov->iov_len;
                iov->iov_len = 0;
            }
            iov++;
            continue;
        }

        assert (iov->iov_len > apdu_offset);

        //  Now is the possibility to join the stream.
        if (!joined) {
            
            //  If offset is no zero have to move iov_base and 
            //  decrease iov_len. 
            if (apdu_offset) {
                iov->iov_base = (unsigned char*)iov->iov_base + apdu_offset;
                iov->iov_len -= apdu_offset;
                
                nbytes -= apdu_offset;
            }

            // Joined the stream.
            joined = true;

            zmq_log (2, "joined into the stream, %s(%i)\n", 
                __FILE__, __LINE__);
        }

        translated += iov->iov_len;
        iov++;
    }

    assert (nbytes == translated);

    return nbytes;
}

size_t zmq::epgm_socket_t::write_one_pkt_with_offset (unsigned char *data_, 
    size_t size_, uint16_t offset_)
{

    //  Put offset information in the buffer.
    put_uint16 (data_, offset_);
   
    //  Send data.
    size_t nbytes = pgm_socket_t::write_one_pkt (data_, size_);

    return nbytes;
}

#endif
