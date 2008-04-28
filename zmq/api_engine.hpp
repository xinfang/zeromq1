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

#include "i_thread.hpp"
#include "msg.hpp"
#include "dispatcher.hpp"
#include "mux.hpp"
#include "demux.hpp"
#include "ypollset.hpp"

namespace zmq
{
    //  Engine to be used from client application thread. NB it is not
    //  thread-safe. In case you want to use 0MQ from several client threads
    //  create api_engine for each of them.

    class api_engine_t : private i_thread
    {
    public:
        //  Creates API engine and attaches it to the command dispatcher
        //  If admin is set to true, engine won't subscribe for any messages
        api_engine_t (dispatcher_t *dispatcher_, bool admin_ = false);

        //  Destroys API engine
        ~api_engine_t ();

        //  Send a message
        void send (void *value_);

        //  Receive a message
        void *receive (bool block = true);

        //  Register the engine with the thread
        //  By registering the engine with the thread you cease the ownership
        //  of the engine to that thread, therefore you shouldn't use it
        //  anymore. If you do, it will result in undefined behaviour.
        void register_engine (i_pollable *engine_, i_thread *thread_);

    private:

        //  i_thread implementation
        int get_thread_id ();
        void send_command (int destination_thread_id_,
            const struct command_t &command_);
        void register_engine (struct i_pollable *engine_);

        void process_commands (ypollset_t::integer_t signals_);
        void process_engine_command (engine_command_t &command_);

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
