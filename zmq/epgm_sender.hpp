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

#ifndef __EPGM_SENDER_HPP_INCLUDED__
#define __EPGM_SENDER_HPP_INCLUDED__

#include <cstdio>

#include "./wire.hpp"
#include "./pgm_sender.hpp"

namespace zmq
{
    class epgm_sender_t : public pgm_sender_t
    {
    public:
        // .
        epgm_sender_t (const char *network_, uint16_t port_);    

        // .
        ~epgm_sender_t ();

        // .
        size_t write (unsigned char *data_, size_t size_, uint16_t offset_);

        // 
        size_t write_pkt (unsigned char *data_, size_t size_, uint16_t offset_);
    private:
        
        struct iovec iov [2];

        unsigned char offset_buff [sizeof (uint16_t)];
    };

}
#endif
