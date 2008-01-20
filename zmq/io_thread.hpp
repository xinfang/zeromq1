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

#ifndef __ZMQ_IO_THREAD_HPP_INCLUDED__
#define __ZMQ_IO_THREAD_HPP_INCLUDED__

#include <stddef.h>
#include <assert.h>
#include <pthread.h>
#include <poll.h>

#include "dispatcher.hpp"
#include "dispatcher_proxy.hpp"
#include "tcp_socket.hpp"
#include "ysocketpair.hpp"
#include "bp_encoder.hpp"
#include "bp_decoder.hpp"

namespace zmq
{

    class io_thread_t 
    {
    public:

        io_thread_t (bool listen_, const char *address_, uint16_t port_,
            dispatcher_t *dispatcher_, int thread_id_, int source_thread_id_,
            int destination_thread_id_, size_t write_buffer_size_,
            size_t read_buffer_size_);
        ~io_thread_t ();

    protected:

        enum {stop_event = 0xff};

        static void *worker_routine (void *arg_);
        void loop ();

        size_t write_buffer_size;
        size_t read_buffer_size;

        unsigned char *out_buf;
        size_t out_buf_size;
        size_t out_buf_pos;

        bp_encoder_t encoder;
        bp_decoder_t decoder;
        dispatcher_proxy_t proxy;
        ysocketpair_t signaler;
        tcp_socket_t socket;
        pthread_t worker;
    };

}

#endif
