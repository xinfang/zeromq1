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

#include <string>
#include <map>

#include "i_engine.hpp"
#include "i_context.hpp"
#include "tcp_socket.hpp"

namespace zmq
{

    //  TODO: remove the circular reference between the dispatcher
    //  and the locator
    class dispatcher_t;

    class locator_t
    {
    public:

        //  Creates the local locator and connects it to the global locator
        //  identified by 'address' and 'port' paramters.
        locator_t (dispatcher_t *dispatcher_, const char *address_,
            uint16_t port_);
        ~locator_t ();

        void add_exchange (const char *exchange_,
            i_context *context_, i_engine *engine_, bool exclusive_ = true,
            const char *address_ = NULL, uint16_t port_ = 0);

        void get_exchange (const char *exchange_,
            i_context **context_, i_engine **engine_);

        void add_queue (const char *exchange_,
            i_context *context_, i_engine *engine_, bool exclusive_ = true,
            const char *address_ = NULL, uint16_t port_ = 0);

        void get_queue (const char *exchange_,
            i_context **context_, i_engine **engine_);

    private:

        enum
        {
            add_exchange_id = 1,
            add_queue_id = 2,
            get_exchange_id = 3,
            get_queue_id = 4
        };

        dispatcher_t *dispatcher;

        struct exchange_info_t
        {
            i_context *context;
            i_engine *engine;
        };

        typedef std::map <std::string, exchange_info_t> exchanges_t;

        exchanges_t exchanges;

        struct queue_info_t
        {
            i_context *context;
            i_engine *engine;
        };

        typedef std::map <std::string, queue_info_t> queues_t;

        queues_t queues;

        //  Access to the locator is synchronised using mutex. That should be
        //  OK as locator is not accessed on the critical path (message being
        //  passed through the system).
        pthread_mutex_t sync;

        tcp_socket_t global_locator;
    };

}

#endif

