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

#ifndef __ZMQ_I_LOCATOR_HPP_INCLUDED__
#define __ZMQ_I_LOCATOR_HPP_INCLUDED__

#include <stddef.h>

namespace zmq
{

    //  Enumerates object types stored in the directory service.
    //  'type_id_count' holds number of exisitng type IDs.
    enum
    {
        exchange_type_id = 0,
        queue_type_id = 1,
        type_id_count = 2
    };

    struct i_locator
    {
        virtual ~i_locator () {};

        virtual void register_endpoint (unsigned char type_id_,
            const char *name_, const char *location_) = 0;
        virtual void resolve_endpoint (unsigned char type_id_,
            const char *name_, char *location_, size_t location_size_) = 0;
    };

}

#endif

