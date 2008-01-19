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

#ifndef __ZMQ_API_THREAD_HPP_INCLUDED__
#define __ZMQ_API_THREAD_HPP_INCLUDED__

#include <stddef.h>
#include <assert.h>

#include "cmsg.hpp"
#include "dispatcher.hpp"
#include "dispatcher_proxy.hpp"
#include "ypollset.hpp"

namespace zmq
{

    class api_thread_t
    {
    public:
        api_thread_t (int thread_id_, dispatcher_t *dispatcher_);

        void send (int destination_thread_id_, const cmsg_t &value_);
        void receive (cmsg_t *value_);

    protected:

        int thread_count;
        int current_thread;
        int poll_frequency;
        int ticks_to_poll;
        dispatcher_proxy_t proxy;
        ypollset_t pollset;
    };

}

#endif
