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

#ifndef __ZMQ_PGM_RECEIVER_ENGINE_HPP_INCLUDED__
#define __ZMQ_PGM_RECEIVER_ENGINE_HPP_INCLUDED__

#include "i_pollable.hpp"
#include "bp_decoder.hpp"
#include "epgm_socket.hpp"

#include <iostream>

namespace zmq
{

    class pgm_receiver_engine_t : public i_pollable
    {
    public:

        pgm_receiver_engine_t (i_thread *calling_thread_, i_thread *thread_,
            const char *iface_, const char *mcast_group_, uint16_t port_, 
            size_t readbuf_size_);
        ~pgm_receiver_engine_t ();

        //  i_pollable interface implementation
        void register_event (i_poller *poller_);
        void in_event ();//(pollfd *pfd_, int count_, int index_);
        void out_event ();//(pollfd *pfd_, int count_, int index_);
        void error_event ();
        void unregister_event ();
        void process_command (const engine_command_t &command_);
    private:
        //  Callback to poller.
        i_poller *poller;

        //  demux & bp_decoder
        demux_t demux;
        bp_decoder_t decoder;
       
        //  PGM socket
        epgm_socket_t epgm_socket;

        iovec *iov;
        size_t iov_len;

        //  Poll handle associated with PGM socket
        int socket_handle;

        //  Poll handle associated with engine PGM waiting pipe
        int pipe_handle;
    };

}

#endif
