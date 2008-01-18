/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __ZMQ_WIRE_HPP_INCLUDED__
#define __ZMQ_WIRE_HPP_INCLUDED__

namespace zmq
{

    //  Writes the size to the buffer as a 64-bit network-byte-order value.
    inline void put_size_64 (unsigned char *buffer_, size_t size_)
    {
        buffer_ [7] = size_ & 0xff;
        size_ >>= 8;
        buffer_ [6] = size_ & 0xff;
        size_ >>= 8;
        buffer_ [5] = size_ & 0xff;
        size_ >>= 8;
        buffer_ [4] = size_ & 0xff;
        size_ >>= 8;
        buffer_ [3] = size_ & 0xff;
        size_ >>= 8;
        buffer_ [2] = size_ & 0xff;
        size_ >>= 8;
        buffer_ [1] = size_ & 0xff;
        size_ >>= 8;
        buffer_ [0] = size_ & 0xff;
    }

    //  Reads the size (64-bit network-byte-order value) from the buffer.
    inline size_t get_size_64 (unsigned char *buffer_)
    {
        size_t size = 0;
        size |= buffer_ [0];
        size <<= 8;
        size |= buffer_ [1];
        size <<= 8;
        size |= buffer_ [2];
        size <<= 8;
        size |= buffer_ [3];
        size <<= 8;
        size |= buffer_ [4];
        size <<= 8;
        size |= buffer_ [5];
        size <<= 8;
        size |= buffer_ [6];
        size <<= 8;
        size |= buffer_ [7];
        return size;
    }

}

#endif
