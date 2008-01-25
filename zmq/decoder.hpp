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

#ifndef __ZMQ_DECODER_HPP_INCLUDED__
#define __ZMQ_DECODER_HPP_INCLUDED__

#include <stddef.h>
#include <algorithm>

#include "dispatcher.hpp"
#include "dispatcher_proxy.hpp"

namespace zmq
{

    template <typename T> class decoder_t
    {
    public:

        inline decoder_t (dispatcher_proxy_t *proxy_,
              int destination_thread_id_) :
            proxy (proxy_),
            destination_thread_id (destination_thread_id_),
            read_ptr (NULL),
            to_read (0),
            next (NULL)
        {
        }

        //  The data pointed to by 'data' parameter are to be managed
        //  by the decoder engine. Decoder engine may manage data lifetime
        //  by setting appropriate deallocation functions to the outgoing
        //  canonical messages. The data should be deallocated using
        //  function pointer to by 'ffn' parameter.
        inline void write (unsigned char *data_, size_t size_, free_fn *ffn_)
        {
            size_t pos = 0;
            while (true) {
                size_t to_copy = std::min (to_read, size_ - pos);
                if (read_ptr) {
                    memcpy (read_ptr, data_ + pos, to_copy);
                    read_ptr += to_copy;
                }
                pos += to_copy;
                to_read -= to_copy;
                if (!to_read)
                    (static_cast <T*> (this)->*next) ();
                if (pos == size_)
                    break;
            }
            proxy->flush ();
            if (ffn_)
                ffn_ (data_);
        }

    protected:

        typedef void (T::*parse_step_t) ();

        inline void next_step (void *read_ptr_, size_t to_read_,
            parse_step_t next_)
        {
            read_ptr = (unsigned char*) read_ptr_;
            to_read = to_read_;
            next = next_;
        }

        inline void done (cmsg_t &msg_)
        {
            proxy->write (destination_thread_id, msg_);
        }

    private:

        dispatcher_proxy_t *proxy;
        int destination_thread_id;
        size_t to_read;
        unsigned char *read_ptr;
        parse_step_t next;
    };

}

#endif
