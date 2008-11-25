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
*/

#ifndef __ZMQ_SERVER_PROTOCOL_HPP_INCLUDED__
#define __ZMQ_SERVER_PROTOCOL_HPP_INCLUDED__

namespace zmq
{

    //  Enumerates individual commands passed between process locator and
    //  global locator. Maps the symbolic names to command IDs.
    enum
    {
        create_id = 1,
        create_ok_id = 2,
        get_id = 3,
        get_ok_id = 4,
        fail_id = 5
    };
    
    //  Enumerates object types sorted in the directory service.
    //  'type_id_count' holds number of exisitng type IDs.
    enum
    {
        exchange_type_id = 0,
        queue_type_id = 1,
        type_id_count = 2
    };

}

#endif
