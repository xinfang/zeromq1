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
#include "../../zmq/api_thread.hpp"
#include "../../zmq/bp_engine.hpp"
#include "../../zmq/poll_thread.hpp"

namespace perf
{

    class zmq_t : public i_transport
    {
    public:
        zmq_t (bool listen_, const char *ip_address_, unsigned short port_, int count_poll_thread_ = 1) :
            dispatcher (1 + count_poll_thread_), count_poll_thread (count_poll_thread_), active_poll_thread (0), 
            api (&dispatcher, 0)
        {
            engine = new zmq::bp_engine_t* [count_poll_thread_];
            assert (engine);

            io = new zmq::poll_thread_t* [count_poll_thread_];
            assert (io);

            for (int i = 0; i < count_poll_thread_; i++) {
//                engine [i] = new zmq::bp_engine_t (listen_, ip_address_, port_ + i, 0, 0, WRITE_BUFFER_SIZE, READ_BUFFER_SIZE);
                engine [i] = new zmq::bp_engine_t (&dispatcher, 1 + i, listen_, ip_address_, port_ + i, 0, 0, 8192, 8192);
                assert (engine [i]);

                io [i] = new zmq::poll_thread_t (engine [i]);
                assert (io [i]);

                if (!listen_ && count_poll_thread > 1) {
                    usleep (1000);  // wait for 'local' 
                }
            }

        }

        inline ~zmq_t ()
        {
            for (int i = 0; i < count_poll_thread; i++) {
                delete io [i];
                delete engine [i];
            }
            delete [] engine;
            delete [] io;
        }

        inline virtual void send (size_t size_)
        {
            assert (size_ <= 65536);
            zmq::cmsg_t msg = {buffer, size_, NULL};
            api.send (1 + active_poll_thread, msg);
            active_poll_thread++;
            active_poll_thread %= count_poll_thread;
        }

        inline virtual size_t receive ()
        {
            zmq::cmsg_t msg;
            api.receive (&msg);
            size_t res = msg.size;
            zmq::free_cmsg (msg);
            return res;
        }

    protected:
        zmq::dispatcher_t dispatcher;
        zmq::api_thread_t api;
        zmq::bp_engine_t **engine;
        zmq::poll_thread_t **io;
        unsigned char buffer [65536];
        unsigned int count_poll_thread;
        unsigned int active_poll_thread;
    };

}

#endif
