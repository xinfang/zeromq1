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

#ifndef __EPGM_SOCKET_HPP_INCLUDED__
#define __EPGM_SOCKET_HPP_INCLUDED__

#include <cstdio>

#include "./wire.hpp"
#include "./pgm_socket.hpp"

namespace zmq
{
    class epgm_socket_t : public pgm_socket_t
    {
    public:
        // .
        epgm_socket_t (bool receiver_, bool pasive_, const char *iface_, 
              const char *mcast_group_, uint16_t port_, 
              size_t readbuf_size_ = 0);

        // .
        ~epgm_socket_t ();

        // 
        size_t write_one_pkt_with_offset (unsigned char *data_, size_t size_, 
            uint16_t offset_);

        //
        size_t read_one_pkt_with_offset (iovec *iov_);

        //
        size_t read_pkt_with_offset (iovec *iov_, size_t iov_len_);

    private:
        
        // Used in receiver

        // Offset in received apdu
        uint16_t apdu_offset;

        // Joined in to the messages stream
        bool joined;
    };
}
#endif
