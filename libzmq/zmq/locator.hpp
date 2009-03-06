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

#ifndef __ZMQ_LOCATOR_HPP_INCLUDED__
#define __ZMQ_LOCATOR_HPP_INCLUDED__

#include <zmq/i_locator.hpp>
#include <zmq/export.hpp>
#include <zmq/tcp_socket.hpp>

namespace zmq
{

    //  Locator uses zmq_server to store and retrieve global object location.

    class locator_t : public i_locator
    {
    public:

        //  Creates the local locator and connects it to the global locator.
        //  If hostname_ is NULL, no global locator is used. Objects not found
        //  on process level are reported as unknown in that case.
        ZMQ_EXPORT locator_t (const char *hostname_ = NULL);

        //  Destroys the locator.
        ZMQ_EXPORT ~locator_t ();

        void register_endpoint (const char *name_, const char *location_);
        void resolve_endpoint (const char *name_, char *location_,
            size_t location_size_);

    private:

        //  Connection to the global locator.
        tcp_socket_t *global_locator;

        locator_t (const locator_t&);
        void operator = (const locator_t&);
    };

}

#endif

