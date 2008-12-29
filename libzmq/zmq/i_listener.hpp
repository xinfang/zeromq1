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

#ifndef __ZMQ_I_LISTENER_HPP_INCLUDED__
#define __ZMQ_I_LISTENER_HPP_INCLUDED__

#include <zmq/export.hpp>
#include <zmq/i_pollable.hpp>

namespace zmq
{
    //  Virtual interface to be exposed by listener engines. It allows listener
    //  to modify the arguments string (say by selecting an unused port).

    struct i_listener : public i_pollable
    {
        ZMQ_EXPORT virtual ~i_listener () {};

        //  Returns modified arguments string.
        virtual const char *get_arguments () = 0;

    };

}

#endif
