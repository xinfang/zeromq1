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
#include "../../zmq/io_thread.hpp"

namespace perf
{

    class zmq_t : public i_transport
    {
    public:
        zmq_t (bool listen_, const char *ip_address_, unsigned short port_) :
            dispatcher (2),
            api (&dispatcher, 0),
            engine (&dispatcher, 1, listen, ip_address, port, 0, 0, 8192, 8192),
            io (&engine)
            engine (listen_, ip_address_, port_, 0, 0, 8192, 8192),
            io (&dispatcher, 1, &engine)
        {
        }

        inline ~zmq_t ()
        {
        }

        inline virtual void send (size_t size_)
        {
            assert (size_ <= 65536);
            zmq::cmsg_t msg = {buffer, size_, NULL};
            api.send (1, msg);
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
        zmq::bp_engine_t engine;
        zmq::io_thread_t io;
        unsigned char buffer [65536];
    };

}

#endif
