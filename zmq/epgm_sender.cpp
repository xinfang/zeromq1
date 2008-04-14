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

#include "./epgm_sender.hpp"

zmq::epgm_sender_t::epgm_sender_t (const char *network_, uint16_t port_) :
    pgm_sender_t (network_, port_)
{
    iov [0].iov_base = offset_buff;
    iov [0].iov_len = sizeof (uint16_t);
}

zmq::epgm_sender_t::~epgm_sender_t ()
{
printf ("= %s(%i)\n", __FILE__, __LINE__);
}


size_t zmq::epgm_sender_t::write (unsigned char *data_, size_t size_, uint16_t offset_)
{
    printf ("going to write %iB + %iB, offset %i\n", (int)size_, (int)sizeof (uint16_t), offset_);

    unsigned char *ebuff = new unsigned char [size_ + sizeof (uint16_t)];
    assert (ebuff);

    put_uint16 (ebuff, offset_); 
    memcpy (ebuff + sizeof (uint16_t), data_, size_);

    size_t nbytes = pgm_sender_t::write (ebuff, size_ + sizeof (uint16_t));
    if (nbytes != size_ + sizeof (uint16_t)) {
        printf ("wrote only %iB, \n", (int)nbytes);
        assert (0);
    }

    delete [] ebuff;

    return nbytes;
}

size_t zmq::epgm_sender_t::write_pkt (unsigned char *data_, size_t size_, uint16_t offset_)
{
    printf ("going to write %iB + %iB, offset %i, %s(%i)\n", (int)size_, 
        (int)sizeof (uint16_t), offset_, __FILE__, __LINE__);
    
    put_uint16 (offset_buff, offset_);
    
    iov [1].iov_base = data_;
    iov [1].iov_len = size_;

    size_t nbytes = pgm_sender_t::write_pkt (iov, 2);

    // returning original size, without added offset 
    nbytes = nbytes > 0 ? nbytes - sizeof (uint16_t) : 0;

    printf ("wrote %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

    return nbytes;
}

