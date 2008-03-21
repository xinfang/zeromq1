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

#ifndef __ZMQ_BP_ENGINE_HPP_INCLUDED__
#define __ZMQ_BP_ENGINE_HPP_INCLUDED__

#include "i_pollable.hpp"
#include "bp_encoder.hpp"
#include "bp_decoder.hpp"
#include "tcp_socket.hpp"

namespace zmq
{

    //  bp_engine uses TCP to transport messages in 0MQ backen protocol format.
    //  Event handling is done via poll - i.e. bp_engine should be used with
    //  poll_thread.

    class bp_engine_t : public i_pollable
    {
    public:

        //  Creates bp_engine. Attaches it to dispatcher. Underlying TCP 
        //  connection is initialised using listen, address
        //  and port parameters. source_engine_id specifies which engine
        //  to get messages from to be send to the socket, destination_engine_id
        //  specified which engine to send incoming messages to. writebuf_size
        //  and readbuf_size determine the amount of batching to use.
        bp_engine_t (dispatcher_t *dispatcher_,
            bool listen_, const char *address_, uint16_t port_,
            int source_engine_id_, int destination_engine_id_,
            size_t writebuf_size_, size_t readbuf_size_);
        ~bp_engine_t ();

        //  i_pollable interface implementation
        void set_signaler (i_signaler *signaler_);
        void revive (int engine_id_);
        int get_fd ();
        short get_events ();
        void in_event ();
        void out_event ();

    private:

        dispatcher_proxy_t proxy;   

        unsigned char *writebuf;
        size_t writebuf_size;

        unsigned char *readbuf;
        size_t readbuf_size;

        size_t write_size;
        size_t write_pos;

        bp_encoder_t encoder;
        bp_decoder_t decoder;
        tcp_socket_t socket;

        short events;
    };

}

#endif
