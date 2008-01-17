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

#ifndef __ZMQ_PARSER_HPP_INCLUDED__
#define __ZMQ_PARSER_HPP_INCLUDED__

#include "dispatcher.hpp"
#include "dispatcher_proxy.hpp"

namespace zmq
{

    template <typename T> class parser_t
    {
    public:

        inline parser_t (dispatcher_proxy_t *proxy_,
              int destination_thread_id_) :
            proxy (proxy_),
            destination_thread_id (destination_thread_id_),
            read_pos (NULL),
            to_read (0),
            next (NULL)
        {
        }

        //  The data pointed to by 'data' parameter are to be managed
        //  by the parser engine. Parser engine manages data lifetime
        //  by setting appropriate deallocation functions to the outgoing
        //  canonical messages. The data should be deallocated using
        //  function pointer to by 'ffn' parameter.
        inline void write (unsigned char *data, size_t size, free_fn *ffn)
        {
            size_t pos = 0;
            while (true) {
                size_t to_copy = std::min (to_read, size - pos);
                memcpy (read_pos, data + pos, to_copy);
                read_pos += to_copy;
                pos += to_copy;
                to_read -= to_copy;
                if (!to_read)
                    (static_cast <T*> (this)->*next) ();
                if (pos == size)
                    break;
            }
            if (ffn)
                ffn (data);
        }

    protected:

        typedef void (T::*parse_step_t) ();

        inline void next_step (void *read_pos_, size_t to_read_,
            parse_step_t next_)
        {
            read_pos = (unsigned char*) read_pos_;
            to_read = to_read_;
            next = next_;
        }

        inline void done (cmsg_t &msg)
        {
            proxy->write (destination_thread_id, msg);
        }

    private:

        dispatcher_proxy_t *proxy;
        int destination_thread_id;
        size_t to_read;
        unsigned char *read_pos;
        parse_step_t next;
    };

}

#endif
