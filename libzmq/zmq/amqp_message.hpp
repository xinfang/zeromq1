/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_AMQP_MESSAGE_HPP_INCLUDED__
#define __ZMQ_AMQP_MESSAGE_HPP_INCLUDED__

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_AMQP

#include <zmq/message.hpp>

namespace zmq
{

    class amqp_message_t : public message_t
    {
    public:
        amqp_message_t (const char *exchange_, const char *routing_key_,
            int property_count_, const char **property_names_,
            const char **property_values_, size_t size_);

        inline void *data ()
        {
            return body;
        }

        inline size_t size ()
        {
            return message_t::size () -
                ((unsigned char*) body - (unsigned char*) message_t::data ());
        }

    private:
        void *body;
    };

}

#endif

#endif
