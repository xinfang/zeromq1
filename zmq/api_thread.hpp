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

#ifndef __ZMQ_API_THREAD_HPP_INCLUDED__
#define __ZMQ_API_THREAD_HPP_INCLUDED__

#include <vector>
#include <string>
#include <utility>

#include "config.hpp"
#include "i_context.hpp"
#include "i_engine.hpp"
#include "message.hpp"
#include "dispatcher.hpp"
#include "mux.hpp"
#include "demux.hpp"
#include "ypollset.hpp"
#include "scope.hpp"
#include "i_locator.hpp"

namespace zmq
{
    //  Engine to be used from client application thread. NB it is not
    //  thread-safe. In case you want to use 0MQ from several client threads
    //  create api_thread for each of them.

    class api_thread_t : private i_context, private i_engine
    {
    public:
        //  Creates API engine and attaches it to the command dispatcher and
        //  resource locator.
        static api_thread_t *create (dispatcher_t *dispatcher_,
            i_locator *locator_);

        //  Destroys API engine
        ~api_thread_t ();

        //  Creates new exchange, returns exchange ID
        int create_exchange (
            const char *exchange_,
            scope_t scope_ = scope_local,
            const char *interface_ = NULL,
            poll_thread_t *listener_thread_ = NULL,
            int handler_thread_count_ = 0,
            poll_thread_t **handler_threads_ = NULL);

        //  Creates new queue, returns queue ID
        int create_queue (
            const char *queue_,
            scope_t scope_ = scope_local,
            const char *interface_ = NULL,
            poll_thread_t *listener_thread_ = NULL,
            int handler_thread_count_ = 0,
            poll_thread_t **handler_threads_ = NULL);

        //  Binds the exchange to the queue
        //  If one of the objects cannot be found, function returns false
        //  and no binding is created.
        bool bind (const char *exchange_, const char *queue_,
            poll_thread_t *exchange_thread_, poll_thread_t *queue_thread_);

        //  Send a message to specified exchange, 0MQ takes responsibility
        //  for deallocating the message. If there are any pendign pre-sent
        //  messages, flush them.
        void send (int exchange_id_, message_t &msg_);

        //  Presend the message. The message will be stored internally and
        //  sent only after 'flush' is called. In other respects it behaves
        //  the same as 'send' function.
        void presend (int exchange_id_, message_t &msg_);

        //  Flush all the pre-sent messages.
        void flush ();

        //  Receive a message, if 'block' argument is true, it'll block till
        //  message arrives. It returns ID of the queue message was retrieved
        //  from, 0 is no message was retrieved.
        int receive (message_t *msg_, bool block_ = true);

    private:

        api_thread_t (dispatcher_t *dispatcher_, i_locator *locator_);

        //  i_context implementation
        int get_thread_id ();
        void send_command (i_context *destination_, const command_t &command_);

        //  i_engine implementation
        void process_command (const engine_command_t &command_);

        void process_commands (ypollset_t::integer_t signals_);
        void process_engine_command (engine_command_t &command_);

        int ticks;
        dispatcher_t *dispatcher;
        i_locator *locator;
        int thread_id;
        ypollset_t pollset;

        typedef std::vector <std::pair <std::string, demux_t> > exchanges_t;
        exchanges_t exchanges;

        //  Current queue points to the queue to be used for retrieving the
        //  message next time it is required
        typedef std::vector <std::pair <std::string, mux_t> > queues_t;
        queues_t queues; 
        queues_t::size_type current_queue; 

        //  Time when last command processing was performed (in ticks)
        uint64_t last_command_time;   
    };

}

#endif
