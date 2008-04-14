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

#include "epgm_receiver.hpp"
#include "err.hpp"

zmq::epgm_receiver_t::epgm_receiver_t (const char *network_,
    uint16_t port_): pgm_receiver_t (network_, port_), apdu_offset (0), 
    joined (false)
{

}

zmq::epgm_receiver_t::~epgm_receiver_t ()
{

}

size_t zmq::epgm_receiver_t::read (unsigned char *data_, size_t size_)
{ 
    ssize_t nbytes = pgm_receiver_t::read (data_, size_);
    errno_assert (nbytes != -1);
    return nbytes;

}

size_t zmq::epgm_receiver_t::read_msg (iovec **iov_)
{

    // Read data
    size_t nbytes = pgm_receiver_t::read_msg (iov_);

    // Read APDU offset
    apdu_offset = get_uint16 ((unsigned char*)(*iov_)->iov_base);

    // Shift iov_base & decrease iov_len of the first iovec by 2B
    (*iov_)->iov_base = (unsigned char*)(*iov_)->iov_base + sizeof (uint16_t);
    (*iov_)->iov_len -= sizeof (uint16_t);
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

    // Shiffting
    while (apdu_offset > 0) {
        
        assert (nbytes >= 0 );

        // Offset is bigger than current iov_len
        if (apdu_offset > (*iov_)->iov_len) {
            printf ("throwing iovec (len %i), %s(%i)\n", (int)(*iov_)->iov_len, __FILE__, __LINE__);
            apdu_offset -= (*iov_)->iov_len;
            nbytes -= (*iov_)->iov_len;
            (*iov_++);
            continue;
        }
        
        // Message starts in current iovec
        printf ("shifting iovec (len %i), %s(%i)\n", (int)(*iov_)->iov_len, __FILE__, __LINE__);

        // Shift iov_base
        (*iov_)->iov_base = (unsigned char*)(*iov_)->iov_base + apdu_offset;
        (*iov_)->iov_len -= apdu_offset;
        nbytes -= apdu_offset;
        apdu_offset -= apdu_offset;
    }
  
    // Joined 
    joined = true;

    return nbytes;
}
