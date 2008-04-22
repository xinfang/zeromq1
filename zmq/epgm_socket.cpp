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

#include "./epgm_socket.hpp"

zmq::epgm_socket_t::epgm_socket_t (bool receiver_, bool pasive_, const char *network_, uint16_t port_) :
    pgm_socket_t (receiver_, pasive_, network_, port_), apdu_offset (0), joined (false)
{

}

zmq::epgm_socket_t::~epgm_socket_t ()
{

}

size_t zmq::epgm_socket_t::read_one_pkt_with_offset (iovec *iov_)
{

    // Read data
    size_t nbytes = read_one_pkt (iov_);

    // It was not ODATA event
    if (!nbytes)
        return 0;

    // Read APDU offset
    apdu_offset = get_uint16 ((unsigned char*)iov_->iov_base);

    // Shift iov_base & decrease iov_len of the first iovec by 2B
    iov_->iov_base = (unsigned char*)iov_->iov_base + sizeof (uint16_t);
    iov_->iov_len -= sizeof (uint16_t);
    nbytes -= sizeof (uint16_t);

    printf ("read apdu_offset %i, %s(%i)\n", apdu_offset, __FILE__, __LINE__);
    fflush (stdout);

    if (apdu_offset == 0xFFFF) {
        if (joined) {
            printf ("0xffff, joined sending all up, %s(%i)\n", 
                __FILE__, __LINE__);
            return nbytes;
        } else {
            printf ("0xffff, not joined throwing data, %s(%i)\n", 
                __FILE__, __LINE__);
            return (size_t)0;
        }
    }

    assert (nbytes > apdu_offset);

    if (!joined) {

        printf ("shifting iovec (len %i), %s(%i)\n", (int)iov_->iov_len, __FILE__, __LINE__);

        // Shift iov_base
        iov_->iov_base = (unsigned char*)iov_->iov_base + apdu_offset;
        iov_->iov_len -= apdu_offset;

        // Joined 
        joined = true;

        printf ("joined into the stream, %s(%i)\n", __FILE__, __LINE__);
    }

    return nbytes;
}

size_t zmq::epgm_socket_t::write_one_pkt_with_offset (unsigned char *data_, size_t size_, uint16_t offset_)
{
    printf ("going to write %iB, offset %i, %s(%i)\n", (int)size_, 
        offset_, __FILE__, __LINE__);
    
    put_uint16 (data_, offset_);
    
    size_t nbytes = pgm_socket_t::write_one_pkt (data_, size_);

    nbytes = nbytes > 0 ? nbytes : 0;

    printf ("wrote %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

    return nbytes;
}

