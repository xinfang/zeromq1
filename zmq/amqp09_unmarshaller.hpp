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

    This file is generated from amqp0-9.xml by amqp09.gsl using GSL/4.
*/

#ifndef __ZMQ_AMQP09_UNMARSHALLER_HPP_INCLUDED__
#define __ZMQ_AMQP09_UNMARSHALLER_HPP_INCLUDED__

#include "i_amqp09.hpp"

namespace zmq
{

    //  Unmarshaller class converts binary representation of AMQP commands
    //  to method calls on i_amqp09 interface.

    class amqp09_unmarshaller_t
    {
    public:

        inline amqp09_unmarshaller_t (i_amqp09 *amqp_) :
            amqp (amqp_)
        {
        }

        inline ~amqp09_unmarshaller_t ()
        {
        }

        //  Calls the method associated specified class id and method id
        //  on encapsulated i_amqp09 interface. Parameter for the method are
        //  parsed from the binary data specified by 'args' and 'args_size'
        //  arguments.
        void write (uint16_t class_id, uint16_t method_id,
            unsigned char *args, size_t args_size);

    private:

        //  Helper function to convert binary field table into field_table_t
        void get_field_table (unsigned char *args, size_t args_size,
            size_t offset, i_amqp09::field_table_t &table_);

        i_amqp09 *amqp;
    };

}
#endif
