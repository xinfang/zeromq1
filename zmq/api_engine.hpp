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

#include <list>

#include "i_context.hpp"
#include "i_engine.hpp"
#include "msg.hpp"
#include "dispatcher.hpp"
#include "mux.hpp"
#include "demux.hpp"
#include "ypollset.hpp"
#include "scope.hpp"

namespace zmq
{
    //  Engine to be used from client application thread. NB it is not
    //  thread-safe. In case you want to use 0MQ from several client threads
    //  create api_engine for each of them.

    class api_engine_t : private i_context, private i_engine
    {
    public:
        //  Creates API engine and attaches it to the command dispatcher
        //  If admin is set to true, engine won't subscribe for any messages
        api_engine_t (dispatcher_t *dispatcher_);

        //  Destroys API engine
        ~api_engine_t ();

        //  Creates new exchange
        void create_exchange (
            const char *exchange_,
            scope_t scope_ = scope_local,
            const char *address = NULL,
            uint16_t port_ = 0,
            poll_thread_t *listener_thread_ = NULL,
            int handler_thread_count_ = 0,
            poll_thread_t **handler_threads_ = NULL);

        //  Creates new queue
        void create_queue (
            const char *queue_,
            scope_t scope_ = scope_local,
            const char *address_ = NULL,
            uint16_t port_ = 0,
            poll_thread_t *listener_thread_ = NULL,
            int handler_thread_count_ = 0,
            poll_thread_t **handler_threads_ = NULL);

        //  Binds the exchange to the queue
        void bind (const char *exchange_, const char *queue_,
            poll_thread_t *exchange_thread_, poll_thread_t *queue_thread_);

        //  Send a message to specified exchange, 0MQ takes responsibility
        //  for deallocating the message.
        void send (const char *exchange_, void *value_);

        //  Receive a message, if 'block' argument is true, it'll block till
        //  message arrives. If it is false, it returns immediately. If no
        //  message is available the return value is NULL.
        void *receive (bool block = true);

    private:

        //  i_context implementation
        int get_thread_id ();
        void send_command (i_context *destination_, const command_t &command_);

        //  i_engine implementation
        void process_command (const engine_command_t &command_);

        void process_commands (ypollset_t::integer_t signals_);
        void process_engine_command (engine_command_t &command_);

        enum {max_ticks = 100};

        int ticks;
        dispatcher_t *dispatcher;
        int thread_id;
        ypollset_t pollset;

        typedef std::map <std::string, demux_t> exchanges_t;
        exchanges_t exchanges;

        typedef std::map <std::string, mux_t> queues_t;
        queues_t queues;        
    };

}

#endif
