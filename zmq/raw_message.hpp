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

#include <assert.h>

#include "stdint.hpp"
#include "config.hpp"
#include "atomic_counter.hpp"

namespace zmq
{

    //  Prototype for the message body deallocation functions.
    //  It is deliberately defined in the way to comply with standard C free.
    typedef void (free_fn) (void *data_);


    //  Shared message buffer. Message data are either allocated in one block
    //  with this structure - thus avoiding one malloc/free pair - or they are
    //  stored in used-supplied memory. In the latter case, ffn member stores
    //  pointer to the function to be used to deallocate the data.

    struct message_content_t
    {
        void *data;
        size_t size;
        free_fn *ffn;
        bool shared;
        atomic_counter_t refcount;
    };

    //  POD version of 0MQ message. The point is to avoid any implicit
    //  construction/destruction/copying of the structure.
    //  From user's point of view raw message is wrapped in message_t
    //  class which makes its usage more convenient.

    struct raw_message_t
    {
        enum {
            delimiter_tag = 0,
            vsm_tag = 1
        };

        message_content_t *msg;
        uint16_t vsm_size;
        unsigned char vsm_data [max_vsm_size];
    };

    //  Allocates a message of the specified size.
    inline message_content_t *msg_alloc (size_t size_)
    {
        message_content_t *msg =
            (message_content_t*) malloc (sizeof (message_content_t) + size_);
        assert (msg);
        msg->data = (void*) (msg + 1);
        msg->size = size_;
        msg->ffn = NULL;
        msg->shared = false;
        msg->refcount.set (1);
        return msg;
    }

    //  Creates a message from the supplied buffer. From this point on
    //  message takes care of buffer's lifetime - it will deallocate it
    //  once it is not needed. This functionality is useful to avoid copying
    //  data in case you are dealing with legacy code and cannot se msg_alloc.
    //  In other cases, however, msg_alloc should be prefered as it is more
    //  efficient when compared to msg_attach.
    inline message_content_t *msg_attach (void *data_, size_t size_,
        free_fn *ffn_)
    {
        message_content_t *msg =
            (message_content_t*) malloc (sizeof (message_content_t));
        assert (msg);
        msg->data = data_;
        msg->size = size_;
        msg->ffn = ffn_;
        msg->shared = false;
        msg->refcount.set (1);
        return msg;
    }

    //  Returns pointer to the message body
    inline void *msg_data (message_content_t *msg_)
    {
        return msg_->data;
    }

    //  Returns message body size
    inline size_t msg_size (message_content_t *msg_)
    {
        return msg_->size;
    }

    //  Don't rely on the function returning same pointer that was passed as
    //  an argument. This is implementation-specific and may change
    //  in the future.
    inline message_content_t *msg_unsafe_copy (message_content_t *msg_)
    {
        msg_->refcount.add (1);
        return msg_;
    }

    //  Don't rely on the function returning same pointer that was passed as
    //  an argument. This is implementation-specific and may change
    //  in the future.
    inline message_content_t *msg_safe_copy (message_content_t *msg_)
    {
        msg_->refcount.add (1);
        msg_->shared = true;
        return msg_;
    }

    //  Releases the particular copy of the message.
    inline void msg_dealloc (message_content_t *msg_)
    {
        if (!msg_)
            return;

        if (!(msg_->shared ? msg_->refcount.sub (1) :
              msg_->refcount.sub (1))) {
            if (msg_->ffn)
                msg_->ffn (msg_->data);
            free (msg_);
        }
    }

}

#endif
