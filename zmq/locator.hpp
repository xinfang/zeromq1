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
*/

#ifndef __ZMQ_LOCATOR_HPP_INCLUDED__
#define __ZMQ_LOCATOR_HPP_INCLUDED__

#include <vector>

#include "i_pollable.hpp"
#include "i_context.hpp"

namespace zmq
{

    //  TODO: remove the circular reference between the dispatcher
    //  and the locator
    class dispatcher_t;

    class locator_t
    {
    public:

        locator_t (dispatcher_t *dispatcher_);
        ~locator_t ();

        void register_engine (i_context *context_, i_pollable *engine_);

    private:

        dispatcher_t *dispatcher;

        struct engine_info_t
        {
            i_context *context;
            i_pollable *engine;
        };

        std::vector <engine_info_t> engines;

        //  Access to the locator is synchronised using mutex. That should be
        //  OK as locator is not accessed on the critical path (message being
        //  passed through the system).
        pthread_mutex_t sync;
    };

}

#endif

