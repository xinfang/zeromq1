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

#include "dispatcher.hpp"
#include "parser.hpp"

namespace zmq
{

    class bp_parser_t : public parser_t <bp_parser_t>
    {
    public:

        bp_parser_t (unsigned char routing_tag_);

    protected:

        void one_byte_size_ready ();
        void eight_byte_size_ready ();
        void message_ready ();

        unsigned char routing_tag;
        unsigned char tmpbuf [8];
        dispatcher_t::item_t *msg;
    };

}

#endif
