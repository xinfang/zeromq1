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

#ifndef __PERF_ZEROMQ_HPP_INCLUDED__
#define __PERF_ZEROMQ_HPP_INCLUDED__

#include "../interfaces/i_transport.hpp"

#include "../../zmq/dispatcher.hpp"
#include "../../zmq/api_engine.hpp"
#include "../../zmq/bp_engine.hpp"
#include "../../zmq/poll_thread.hpp"

namespace perf
{

    class zmq_t : public i_transport
    {
    public:
        zmq_t (bool sender_, const char *queue_name_, const char *exchange_name_, 
              const char *locator_ip_, unsigned short locator_port_,
              const char *listen_ip_, unsigned short listen_port_, 
              unsigned int thread_count_ = 2) :
            thread_count (thread_count_), sender (sender_),
            dispatcher (thread_count),
            locator (locator_ip_, locator_port_),
            api (&dispatcher, &locator),
            worker (&dispatcher)
        {
//            printf ("locator %s/%i, listen %s/%i\n", locator_ip_, locator_port_, 
//                listen_ip_, listen_port_);


            zmq::poll_thread_t *workers [] = {&worker};

            if (sender) {
                assert (!listen_ip_);
                assert (!listen_port_);

                // create & bind local exchange
                exchange_id = api.create_exchange ("E_LOCAL");
                api.bind ("E_LOCAL", queue_name_, &worker, &worker);
                
                // create & bind local queue
                api.create_queue ("Q_LOCAL");
                api.bind (exchange_name_, "Q_LOCAL", &worker, &worker);

            } else {
                assert (listen_ip_);
                assert (listen_port_);
                
                api.create_queue (queue_name_, zmq::scope_global, listen_ip_, listen_port_,
                    &worker, 1, workers);

                exchange_id = api.create_exchange (exchange_name_, zmq::scope_global, listen_ip_, listen_port_ + 1,
                    &worker, 1, workers);

            }
        }

        inline ~zmq_t ()
        {
            sleep (1);
        }

        inline virtual void send (size_t size_, unsigned int thread_id_ = 0)
        {
            assert (thread_id_ < thread_count);
            assert (size_ <= 65536);

            void *msg = zmq::msg_alloc (size_);
            api.send (exchange_id, msg);
        }

        inline virtual void send_sync_message (void)
        {
            //assert (!sender);

            void *msg = zmq::msg_alloc (1);
            api.send (exchange_id, msg);
        }

        inline virtual size_t receive (unsigned int thread_id_ = 0)
        {
            assert (thread_id_ < thread_count);

            void *msg = api.receive ();
            assert (msg);

            size_t size = ((zmq::msg_t*)msg)->size;
            
            zmq::msg_dealloc (msg);

            return size;
        }

        inline virtual size_t receive_sync_message (void)
        {
            //assert (sender);

            void *msg = api.receive ();
            assert (msg);

            size_t size = ((zmq::msg_t*)msg)->size;
            
            zmq::msg_dealloc (msg);

            assert (size == 1);;

            return size;
        }

    protected:

        unsigned int thread_count;
        bool sender;
        zmq::dispatcher_t dispatcher;
        zmq::locator_t locator;
        zmq::api_engine_t api;
        zmq::poll_thread_t worker;
	int exchange_id;
    };

}

#endif
