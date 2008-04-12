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

#ifndef __ZMQ_POLL_THREAD_HPP_INCLUDED__
#define __ZMQ_POLL_THREAD_HPP_INCLUDED__

#include <stddef.h>
#include <assert.h>
#include <pthread.h>
#include <poll.h>

#include "dispatcher.hpp"
#include "ysocketpair.hpp"
#include "i_pollable.hpp"

namespace zmq
{

    //  Poll thread is a worker thread that wait for events from engines
    //  using POSIX poll function and schedules handling of the signals
    //  by individual engines. Engine compatible with poll thread should
    //  expose i_pollable interface.

    class poll_thread_t 
    {
    public:

        //  Create a poll thread to serve specified engine. At the moment only
        //  a single engine can be attached to the poll thread. In the future
        //  poll thread should be able to handle multiple engines.
        poll_thread_t (dispatcher_t *dispatcher_, i_pollable *engine_);
        ~poll_thread_t ();

    private:

        enum {stop_event = 30};

        static void *worker_routine (void *arg_);
        void loop ();

        dispatcher_t *dispatcher;
        int thread_id;

        i_pollable *engine;
        ysocketpair_t signaler;
        pthread_t worker;
    };

}

#endif
