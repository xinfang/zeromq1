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

#ifndef __ZMQ_API_ENGINE_HPP_INCLUDED__
#define __ZMQ_API_ENGINE_HPP_INCLUDED__

#include "cmsg.hpp"
#include "dispatcher.hpp"
#include "mux.hpp"
#include "demux.hpp"
#include "ypollset.hpp"

namespace zmq
{
    //  Engine to be used from client application thread. NB it is not
    //  thread-safe. In case you want to use 0MQ from several client threads
    //  create api_engine for each of them.

    class api_engine_t
    {
    public:
        //  Creates API engine and attaches it to the command dispatcher
        api_engine_t (dispatcher_t *dispatcher_);

        //  Destroys API engine
        ~api_engine_t ();

        //  Send a message
        void send (const cmsg_t &value_);

        //  Receive a message
        void receive (cmsg_t *value_);

    protected:

        void process_commands (ypollset_t::integer_t signals);

        enum {max_ticks = 100};

        int ticks;
        mux_t mux;
        demux_t demux;
        dispatcher_t *dispatcher;
        int thread_id;
        ypollset_t pollset;
    };

}

#endif
