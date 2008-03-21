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
        zmq_t (bool listen_, const char *ip_address_, unsigned short port_, 
                unsigned int count_io_thread_ = 1) :
            dispatcher (2 * count_io_thread_), 
            count_io_thread (count_io_thread_) 
        {
            engine = new zmq::bp_engine_t* [count_io_thread_];
            assert (engine);

            io = new zmq::poll_thread_t* [count_io_thread_];
            assert (io);

            api = new zmq::api_engine_t* [count_io_thread_];
            assert (api);

            for (unsigned int i = 0; i < count_io_thread_; i++) {
                
                // api IDs are from 0 to count_io_thread_
                api [i] = new zmq::api_engine_t (&dispatcher); 
                assert (api [i]);
           }

           for (unsigned int i = 0; i < count_io_thread_; i++) {

                // engine IDs are from count_io_thread_ to 2 * count_io_thread_
                engine [i] = new zmq::bp_engine_t (&dispatcher, 
                    listen_, ip_address_, port_ + i, i, i, 8192, 8192);
                assert (engine [i]);
            }

           for (unsigned int i = 0; i < count_io_thread_; i++) {
                io [i] = new zmq::poll_thread_t (engine [i]);
                assert (io [i]);

                if (!listen_ && count_io_thread > 1) {
                    usleep (1000);  // wait for 'local' 
                }
            }

        }

        inline ~zmq_t ()
        {
            for (unsigned int i = 0; i < count_io_thread; i++) {
                delete io [i];
                delete engine [i];
                delete api [i];
            }
            delete [] engine;
            delete [] io;
            delete [] api;
        }

        inline virtual void send (size_t size_, unsigned int thread_id_ = 0)
        {
            assert (thread_id_ < count_io_thread);
            assert (size_ <= 65536);

            zmq::cmsg_t msg = {buffer, size_, NULL};
            api [thread_id_]->send (count_io_thread + thread_id_, msg);
        }

        inline virtual size_t receive (unsigned int thread_id_ = 0)
        {
            assert (thread_id_ < count_io_thread);

            zmq::cmsg_t msg;
            api [thread_id_]->receive (&msg);
            size_t res = msg.size;
            zmq::free_cmsg (msg);
            return res;

        }

    protected:
        zmq::dispatcher_t dispatcher;
        zmq::api_engine_t **api;
        zmq::bp_engine_t **engine;
        zmq::poll_thread_t **io;
        unsigned char buffer [65536];
        unsigned int count_io_thread;
    };

}

#endif
