/*
    Copyright (c) 2007-2008 FastMQ Inc.

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

    This file is generated from amqp0-9-1.xml by amqp.gsl using GSL/4.
*/

#ifndef __ZMQ_AMQP_UNMARSHALLER_HPP_INCLUDED__
#define __ZMQ_AMQP_UNMARSHALLER_HPP_INCLUDED__

#if defined ZMQ_HAVE_AMQP

#include <zmq/i_amqp.hpp>

namespace zmq
{

    //  Unmarshaller class converts binary representation of AMQP commands
    //  to method calls on i_amqp interface.

    class amqp_unmarshaller_t
    {
    public:

        //  'callback' is an object who's method is to be called when
        //  AMQP command is decoded.
        inline amqp_unmarshaller_t (i_amqp *callback_) :
            callback (callback_)
        {
        }

        inline ~amqp_unmarshaller_t ()
        {
        }

        //  Calls the method associated specified class id and method id
        //  on encapsulated i_amqp interface. Parameter for the method are
        //  parsed from the binary data specified by 'args' and 'args_size'
        //  arguments.
        void write (uint16_t class_id, uint16_t method_id,
            unsigned char *args, size_t args_size);

    private:

        //  Helper function to convert binary field table into field_table_t
        void get_field_table (unsigned char *args, size_t args_size,
            size_t offset, i_amqp::field_table_t &table_);

        //  Object who's method is to be called when AMQP command is decoded.
        i_amqp *callback;
    };

}

#endif

#endif
