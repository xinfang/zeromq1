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

#ifndef __ZMQ_RAW_MESSAGE_HPP_INCLUDED__
#define __ZMQ_RAW_MESSAGE_HPP_INCLUDED__

#include "msg.hpp"
#include "config.hpp"

namespace zmq
{

    //  POD version of message. The point is to avoid any implicit
    //  construction/destruction/copying of the structure in the ypipe.
    //  From user's point of view raw message is wrapped in message_t
    //  class which makes its usage more convenient.

    struct raw_message_t
    {
        enum {
            delimiter_tag = 0,
            vsm_tag = 1
        };

        msg_t *msg;
        uint16_t vsm_size;
        unsigned char vsm_data [max_vsm_size];
    };

}

#endif
