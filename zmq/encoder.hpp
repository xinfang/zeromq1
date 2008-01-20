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

#ifndef __ZMQ_ENCODER_HPP_INCLUDED__
#define __ZMQ_ENCODER_HPP_INCLUDED__

#include <assert.h>
#include <algorithm>

#include "dispatcher_proxy.hpp"
#include "cmsg.hpp"

namespace zmq
{

    template <typename T> class encoder_t
    {
    public:

        encoder_t (dispatcher_proxy_t *proxy_, int source_thread_id_,
              size_t chunk_size_) :
            proxy (proxy_),
            source_thread_id (source_thread_id_),
            chunk_size (chunk_size_)
        {
            init_cmsg (msg);
        }

        ~encoder_t ()
        {
            free_cmsg (msg);
        }

        //  The chunk after being used should be deallocated
        //  using standard 'free' function
        bool read (unsigned char **data_, size_t *size_)
        {
            unsigned char *chunk = (unsigned char*) malloc (chunk_size);
            assert (chunk);
            size_t pos = 0;

            while (true) {
                if (to_write) {
                    size_t to_copy = std::min (to_write, chunk_size - pos);
                    memcpy (chunk + pos, write_pos, to_copy);
                    pos += to_copy;
                    write_pos += to_copy;
                    to_write -= to_copy;
                }
                else {
                    if (!((static_cast <T*> (this)->*next) ()))
                    {
                        *data_ = chunk;
                        *size_ = pos;
                        return false;
                    }
                    continue;
                }
                if (pos == chunk_size)
                    break;
            }
            *data_ = chunk;
            *size_ = pos;
            return true;
        }

    protected:

        typedef bool (T::*parse_step_t) ();

        inline void next_step (void *write_pos_, size_t to_write_,
            parse_step_t next_)
        {
            write_pos = (unsigned char*) write_pos_;
            to_write = to_write_;
            next = next_;
        }

        inline bool fetch ()
        {
            //  TODO: review the deallocation of msg (w.r.t. zero copy)
            free_cmsg (msg);
            init_cmsg (msg);
            return proxy->read (source_thread_id, &msg);
        }

        cmsg_t msg;

    private:
        dispatcher_proxy_t *proxy;
        int source_thread_id;
        size_t chunk_size;
        unsigned char *write_pos;
        size_t to_write;
        parse_step_t next;
    };

}

#endif
