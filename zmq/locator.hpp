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

#ifndef __ZMQ_LOCATOR_HPP_INCLUDED__
#define __ZMQ_LOCATOR_HPP_INCLUDED__

#include <string>
#include <map>

#include "i_locator.hpp"
#include "i_engine.hpp"
#include "i_context.hpp"
#include "mutex.hpp"
#include "tcp_socket.hpp"
#include "scope.hpp"
#include "zmq_server.hpp"

namespace zmq
{

    //  Locator class locates resources in the scope of the process.
    //  If the resource cannot be found, it asks global locator service
    //  to find it on the network.

    class locator_t : public i_locator
    {
    public:

        //  Creates the local locator and connects it to the global locator.
        //  If hostname_ is NULL, no global locator is used. Objects not found
        //  on process level are reported as unknown.
        locator_t (const char *hostname_ = NULL);

        //  Destroys the locator.
        ~locator_t ();

        //  Creates object.
        void create (unsigned char type_id_, const char *object_,
            i_context *context_, i_engine *engine_, scope_t scope_,
            const char *interface_,
            poll_thread_t *listener_thread_, int handler_thread_count_,
            poll_thread_t **handler_threads_);

        //  Gets the engine that handles specified object.
        //  Returns false if the object is unknown.
        bool get (unsigned char type_id_, const char *object_,
            i_context **context_, i_engine **engine_,
            poll_thread_t *thread_, const char *local_object_);

    private:

        //  Info about single object.
        struct object_info_t
        {
            i_context *context;
            i_engine *engine;
        };

        //  Maps object names to object infos.
        typedef std::map <std::string, object_info_t> objects_t;

        //  Array of object maps. Index to the array is the type ID of
        //  the object.
        objects_t objects [type_id_count];

        //  Access to the locator is synchronised using mutex. That should be
        //  OK as locator is not accessed on the critical path (message being
        //  passed through the system). The blocking occurs only in the
        //  application threads as they are creating wiring.
        mutex_t sync;

        //  Connection to the global locator.
        tcp_socket_t *global_locator;

        locator_t (const locator_t&);
        void operator = (const locator_t&);
    };

}

#endif

