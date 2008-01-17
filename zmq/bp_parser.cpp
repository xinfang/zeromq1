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

#include "bp_parser.hpp"
#include "wire.hpp"

zmq::bp_parser_t::bp_parser_t (dispatcher_proxy_t *dispatcher_proxy_,
      int destination_thread_id_) :
    parser_t <bp_parser_t> (dispatcher_proxy_, destination_thread_id_)
{
     next_step (tmpbuf, 1, &bp_parser_t::one_byte_size_ready);
}

void zmq::bp_parser_t::one_byte_size_ready ()
{
    if (*tmpbuf == 0xff)
        next_step (tmpbuf, 8, &bp_parser_t::eight_byte_size_ready);
    else {
        msg.size = *tmpbuf;
        msg.data = malloc (*tmpbuf);
        assert (msg.data);
        msg.ffn = free;

        next_step (msg.data, *tmpbuf, &bp_parser_t::message_ready);
    }
}

void zmq::bp_parser_t::eight_byte_size_ready ()
{
    msg.size = get_size_64 (tmpbuf);
    msg.data = malloc (msg.size);
    assert (msg.data);
    msg.ffn = free;

    next_step (msg.data, msg.size, &bp_parser_t::message_ready);
}

void zmq::bp_parser_t::message_ready ()
{
    done (msg);
    next_step (tmpbuf, 1, &bp_parser_t::one_byte_size_ready);
}

