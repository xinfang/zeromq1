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

namespace zmq
{

    template <typename T> class parser_t
    {
    public:

        inline parser_t () :
            read_pos (NULL),
            next (NULL),
            msgbuf_first (NULL),
            msgbuf_last (NULL)
        {
        }

        inline ~parser_t ()
        {
            while (msgbuf_first) {
                dispatcher_t::item_t *o = msgbuf_first;
                msgbuf_first = o->next;
                free_cmsg (o->value);
                delete o;
            }
        }

        inline void write (iovec *iov, size_t iovlen)
        {
            for (size_t iovec_nbr = 0; iovec_nbr != iovlen; iovec_nbr ++) {
                size_t pos = 0;
                while (true) {
                    size_t to_copy = std::min (to_read,
                        iov [iovec_nbr].iov_len - pos);
                    memcpy (read_pos,
                        ((unsigned char*) iov [iovec_nbr].iov_base) + pos,
                        to_copy);
                    read_pos += to_copy;
                    pos += to_copy;
                    to_read -= to_copy;
                    if (!to_read)
                        (static_cast <T*> (this)->*next) ();
                    if (pos == iov [iovec_nbr].iov_len)
                        break;
                }
            }
        }

        inline void read (dispatcher_t::item_t **first,
            dispatcher_t::item_t **last)
        {
            *first = msgbuf_first;
            *last = msgbuf_last;
            msgbuf_first = NULL;
            msgbuf_last = NULL;
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

        inline void done (dispatcher_t::item_t *msg)
        {
            msg->next = NULL;
            if (msgbuf_last)
                msgbuf_last->next = msg;
            msgbuf_last = msg;
            if (!msgbuf_first)
                msgbuf_first = msg;
        }

    private:

        size_t to_read;
        unsigned char *read_pos;
        parse_step_t next;

        dispatcher_t::item_t *msgbuf_first;
        dispatcher_t::item_t *msgbuf_last;
    };

}

#endif
