/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __ZMQ_NATIVE_API_HPP_INCLUDED__
#define __ZMQ_NATIVE_API_HPP_INCLUDED__

#include <stddef.h>
#include <assert.h>

#include "cmsg.hpp"
#include "dispatcher.hpp"
#include "ypollset.hpp"

namespace zmq
{

    class api_thread_t
    {
    public:
        api_thread_t (int thread_id, dispatcher_t *dispatcher);
        ~api_thread_t ();

        void send (int destination_thread_id, const cmsg_t &value);
        void receive (cmsg_t *value);

    protected:

        struct recvbuf_t
        {
            bool alive;
            dispatcher_t::item_t *first;
            dispatcher_t::item_t *last;
        };

        int thread_count;
        int thread_id;
        int current_thread;
        int threads_alive;
        dispatcher_t *dispatcher;
        ypollset_t pollset;
        recvbuf_t *recvbufs;
    };

}

#endif
