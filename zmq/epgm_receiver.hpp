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

#ifndef __EPGM_RECEIVER_HPP_INCLUDED__
#define __EPGM_RECEIVER_HPP_INCLUDED__

#include <cstdio>

#include "./wire.hpp"
#include "./pgm_receiver.hpp"

namespace zmq
{
    class epgm_receiver_t : public pgm_receiver_t
    {
    public:
        epgm_receiver_t (const char *network_, uint16_t port_);

        //  Closes the transport
        ~epgm_receiver_t ();
        
        //  
        size_t read (unsigned char *data_, size_t size_);
        
        //
        size_t read_msg (iovec **iov_);

    private:
        // Offset in received apdu
        uint16_t apdu_offset;

        // Joined in to the messages stream
        bool joined;
    };

}
#endif
