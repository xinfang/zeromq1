/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __EPGM_SOCKET_HPP_INCLUDED__
#define __EPGM_SOCKET_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <zmq/pgm_socket.hpp>

namespace zmq
{
    //  epgm_socket_t class works with offset of the fist message in received
    //  data chunk from the pgm_socket. offset_ is used to detect possibility 
    //  to join multicast message stream. If offset is equal to 0xFFFF means 
    //  that there is no message beginning the the received data chunk.
    class epgm_socket_t : public pgm_socket_t
    {
    public:
        //  If receiver_ is true PGM transport is not generating SPM packets.
        //  interface_ format: iface;mcast_group:port (eth0;226.0.0.1:7500).
        epgm_socket_t (bool receiver_, const char *interface_, 
              size_t readbuf_size_ = 0);

        //  Closes the transport.
        ~epgm_socket_t ();

        //  Send one APDU with first message offset information. 
        //  Note that first 2 bytes in data_ are used to store the offset_
        //  and thus user data has to start at data_ + sizeof (uint16_t).
        size_t write_one_pkt_with_offset (unsigned char *data_, size_t size_, 
            uint16_t offset_);

        //  Read exactly iov_len_ count APDUs, function returns number
        //  of bytes received. Note that if we did not join message stream 
        //  before and there is not message beginning in the APDUs being 
        //  received iov_len for such a APDUs will be 0.
        size_t read_pkt_with_offset (iovec *iov_, size_t iov_len_);

    private:
        
        // If receiver joined the messages stream.
        bool joined;
    };
}
#endif

#endif
