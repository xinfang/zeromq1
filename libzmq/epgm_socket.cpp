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
    size_t readbuf_size_) : pgm_socket_t (receiver_, interface_, readbuf_size_),
    apdu_offset (0), joined (false)
{

}

zmq::epgm_socket_t::~epgm_socket_t ()
{

}

size_t zmq::epgm_socket_t::read_one_pkt_with_offset (iovec *iov_)
{
    assert (0);
}

size_t zmq::epgm_socket_t::read_pkt_with_offset (iovec *iov_, size_t iov_len_)
{

    unsigned int translated = 0;

    // Read data
    size_t nbytes = read_pkt (iov_, iov_len_);

    // It was not ODATA event
    if (!nbytes)
        return (size_t)0;

    iovec *iov = iov_;
    iovec *iov_end = iov + iov_len_;


    while (translated < nbytes) {
        
        assert (iov <= iov_end);

        // Read APDU offset
        apdu_offset = get_uint16 ((unsigned char*)iov->iov_base);

        // Shift iov_base & decrease iov_len of the iovec by sizeof (uint16_t)
        iov->iov_base = (unsigned char*)iov->iov_base + sizeof (uint16_t);
        iov->iov_len -= sizeof (uint16_t);
        nbytes -= sizeof (uint16_t);

        zmq_log (2, "read apdu_offset %i, %s(%i)\n", apdu_offset, __FILE__, __LINE__);

        if (apdu_offset == 0xFFFF) {

            if (joined) {
                zmq_log (2, "0xffff, joined sending all up, %s(%i)\n", 
                    __FILE__, __LINE__);

                translated += iov->iov_len;

            } else {
                zmq_log (2, "0xffff, not joined throwing data, %s(%i)\n", 
                    __FILE__, __LINE__);

                // setting iov_len to 0 that iov_base is not pushed into encoder
                nbytes -= iov->iov_len;
                iov->iov_len = 0;
            }
            iov++;
            continue;
        }

        assert (iov->iov_len > apdu_offset);

        if (!joined) {

            if (apdu_offset) {
               zmq_log (2, "shifting iov_base (len %i), %s(%i)\n", (int)iov_->iov_len, __FILE__, __LINE__);

                // Shift iov_base
                iov->iov_base = (unsigned char*)iov->iov_base + apdu_offset;
                iov->iov_len -= apdu_offset;
                
                nbytes -= apdu_offset;
            }

            // Joined 
            joined = true;

            zmq_log (2, "joined into the stream, %s(%i)\n", __FILE__, __LINE__);
        }

        translated += iov->iov_len;
        iov++;
    }

    assert (nbytes == translated);

    return nbytes;
}

size_t zmq::epgm_socket_t::write_one_pkt_with_offset (unsigned char *data_, size_t size_, uint16_t offset_)
{
    zmq_log (2, "going to write %iB, offset %i, %s(%i)\n", (int)size_, 
        offset_, __FILE__, __LINE__);
    
    put_uint16 (data_, offset_);
    
    size_t nbytes = pgm_socket_t::write_one_pkt (data_, size_);

    nbytes = nbytes > 0 ? nbytes : 0;

    zmq_log (2, "wrote %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

    return nbytes;
}

#endif
