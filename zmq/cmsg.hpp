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

#ifndef __ZMQ_CMSG_HPP_INCLUDED__
#define __ZMQ_CMSG_HPP_INCLUDED__

#include <stddef.h>

#include "free_fn.hpp"

namespace zmq
{

    //  Canonical message. All messages, no matter what kind of protocol
    //  they are trasferred by are transformed to canonical messages
    //  inside 0MQ.
    //
    //  Canonical message is deliberately designed as POD structure.
    //  That way no unexpected implicit copying of message data may occur.
    //
    //  Each canonical message contains a pointer to function to be used
    //  to deallocate message data. If the pointer in NULL, no deallocation
    //  is done (message body is a static buffer).

    struct cmsg_t
    {
        void *data;
        size_t size;
        free_fn *ffn;
    };

    //  Initialise canonical message
    inline void init_cmsg (cmsg_t &msg_)
    {
        msg_.data = NULL;
        msg_.size = 0;
        msg_.ffn = NULL;
    }

    //  Cleat canonical message
    inline void free_cmsg (cmsg_t &msg_)
    {
        if (msg_.ffn && msg_.data)
            msg_.ffn (msg_.data);
    }

}

#endif
