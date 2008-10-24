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

#ifndef __ZMQ_PGM_SENDER_ENGINE_HPP_INCLUDED__
#define __ZMQ_PGM_SENDER_ENGINE_HPP_INCLUDED__

#include <iostream>

#include "i_pollable.hpp"
#include "bp_encoder.hpp"
#include "epgm_socket.hpp"

namespace zmq
{

    class pgm_sender_engine_t : public i_pollable
    {
    public:

        pgm_sender_engine_t (i_thread *calling_thread_, i_thread *thread_,
            const char *iface_, const char *mcast_group_, uint16_t port_,
            i_thread *peer_thread_, i_engine *peer_engine_, 
            const char *peer_name_);
//        dispatcher_t *dispatcher_, int engine_id_,
//            const char *network_, uint16_t port_,
//            int source_engine_id_);
        ~pgm_sender_engine_t ();

        //  Get multicast group from the PGM transport
        const char *get_interface ();

        //  i_pollable interface implementation
        void register_event (i_poller *poller_);
        void in_event ();//pollfd *pfd_, int count_, int index_);
        void out_event ();//pollfd *pfd_, int count_, int index_);
        void error_event ();
        void unregister_event ();
        void process_command (const engine_command_t &command_);
    private:
        //  Callback to poller.
        i_poller *poller;

        //  The thread pgm_sender_sngine is running in.
        i_thread *thread;

        //  Determine the engine and the object (either exchange or queue)
        //  within the engine to serve as a peer to this engine.
        i_thread *peer_thread;
        i_engine *peer_engine;

        //  mux & bp_encoder
        mux_t mux; 
        bp_encoder_t encoder;

        //  PGM socket
        epgm_socket_t epgm_socket;

        //  Poll handle associated with PGM socket
        int socket_handle;

        unsigned char *txw_slice;

        // Taken from transmit window
        size_t max_tsdu;

        size_t write_size;
        size_t write_pos;

        ssize_t first_message_offest;
    };

}

#endif
