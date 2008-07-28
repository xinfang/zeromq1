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

    //  0MQ message. Don't change the body of the message once you've
    //  copied it - the behaviour would be undefined. Don't change the body
    //  of the message received.

    class message_t : private raw_message_t
    {
    public:

        inline message_t ()
        {
            content = (message_content_t*) raw_message_t::vsm_tag;
            vsm_size = 0;
        }

        inline message_t (size_t size_)
        {
            raw_message_init (this, size_);
        }

        inline message_t (void *data_, size_t size_, free_fn *ffn_)
        {
            raw_message_init (this, data_, size_, ffn_);
        }

        inline ~message_t ()
        {
            raw_message_destroy (this);
        }

        inline void destroy ()
        {
            raw_message_destroy (this);
            raw_message_init_delimiter (this);
        }

        inline void detach ()
        {
            content = (message_content_t*) raw_message_t::vsm_tag;
            vsm_size = 0;
        }

        //  Destroys old content of the message and allocates buffer for the
        //  new message body.
        inline void rebuild (size_t size_)
        {
            raw_message_destroy (this);
            raw_message_init (this, size_);            
        }

        inline void *data ()
        {
            return raw_message_data (this);
        }

        inline size_t size ()
        {
            return raw_message_size (this);
        }

    private:

        //  Disable implicit message copying, so that users won't use shared
        //  messages (less efficient) without being aware of the fact.
        message_t (const message_t&);
    };

}

#endif
