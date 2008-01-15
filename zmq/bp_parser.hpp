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

#ifndef __ZMQ_BP_PARSER_HPP_INCLUDED__
#define __ZMQ_BP_PARSER_HPP_INCLUDED__

#include <stddef.h>
#include <sys/uio.h>
#include <algorithm>

#include "wire.hpp"
#include "dispatcher.hpp"
#include "parser.hpp"

namespace zmq
{

    class bp_parser_t : public parser_t <bp_parser_t>
    {
    public:

        //  The "iov" array itself is owned by the calling thread and should be
        //  deallocated by it. However, the data pointed to by individual iovecs
        //  are to be managed by the parser engine. Parser engine manages
        //  data lifetime by setting appropriate deallocation functions to the
        //  outgoing canonical messages. The data should be deallocated using
        //  standard 'free' function.

        bp_parser_t ()
        {
            next_step (tmpbuf, 1, &bp_parser_t::one_byte_size_ready);
        }

    protected:

        void one_byte_size_ready ()
        {
            if (*tmpbuf == 0xff)
                next_step (tmpbuf, 8, &bp_parser_t::eight_byte_size_ready);
            else {
                msg = new dispatcher_t::item_t;
                assert (msg);
                msg->value.size = *tmpbuf;
                msg->value.data = malloc (*tmpbuf);
                assert (msg->value.data);
                msg->value.ffn = free;

                next_step (msg->value.data, *tmpbuf,
                    &bp_parser_t::message_ready);
            }
        }

        void eight_byte_size_ready ()
        {
            msg = new dispatcher_t::item_t;
            assert (msg);
            msg->value.size = get_size_64 (tmpbuf);
            msg->value.data = malloc (msg->value.size);
            assert (msg->value.data);
            msg->value.ffn = free;

            next_step (msg->value.data, msg->value.size,
                &bp_parser_t::message_ready);
        }

        void message_ready ()
        {
            done (msg);
            next_step (tmpbuf, 1, &bp_parser_t::one_byte_size_ready);
        }

        unsigned char tmpbuf [8];
        dispatcher_t::item_t *msg;
    };

}

#endif
