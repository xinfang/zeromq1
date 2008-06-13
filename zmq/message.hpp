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

#ifndef __ZMQ_MESSAGE_HPP_INCLUDED__
#define __ZMQ_MESSAGE_HPP_INCLUDED__

#include "raw_message.hpp"

namespace zmq
{

    class message_t : private raw_message_t
    {
    public:

        inline message_t ()
        {
            msg = (msg_t*) raw_message_t::vsm_tag;
            vsm_size = 0;
        }

        inline message_t (size_t size_)
        {
            if (size_ <= max_vsm_size) {
                msg = (msg_t*) raw_message_t::vsm_tag;
                vsm_size = size_;
            }
            else
                msg = msg_alloc (size_);
        }

        inline message_t (void *data_, size_t size_, free_fn *ffn_)
        {
            msg = msg_attach (data_, size_, ffn_);
        }

        inline ~message_t ()
        {
            if (msg != (msg_t*) raw_message_t::delimiter_tag &&
                  msg != (msg_t*) raw_message_t::vsm_tag)
                msg_dealloc (msg);
        }

        inline void detach ()
        {
            msg = (msg_t*) raw_message_t::vsm_tag;
            vsm_size = 0;
        }

        //  Destroys old content of the message and allocates buffer for the
        //  new message body.
        inline void rebuild (size_t size_)
        {
            if (msg != (msg_t*) raw_message_t::delimiter_tag &&
                  msg != (msg_t*) raw_message_t::vsm_tag)
                msg_dealloc (msg);

            if (size_ <= max_vsm_size) {
                msg = (msg_t*) raw_message_t::vsm_tag;
                vsm_size = size_;
            }
            else
                msg = msg_alloc (size_);            
        }

        void safe_copy (message_t *message_)
        {
            //  VSM are physically copied
            if (!msg) {
                *message_ = *this;
                return;
            }

            //  Large messages are reference counted
            message_->msg = msg_safe_copy (msg);
        }

        inline void *data ()
        {
            if (msg != (void*) raw_message_t::vsm_tag)
                return msg_data (msg);
            else
                return vsm_data;
        }

        inline size_t size ()
        {
            if (msg != (void*) raw_message_t::vsm_tag)
                return msg_size (msg);
            else
                return vsm_size;
        }

        inline void set_queue_id (int queue_id_)
        {
            //  TODO: implement it!
        }

        //  TODO: copying the message
    };

}

#endif
