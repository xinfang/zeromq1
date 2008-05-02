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

#ifndef __PGM_SOCKET_HPP_INCLUDED__
#define __PGM_SOCKET_HPP_INCLUDED__

#include <glib.h>
#include <pgm/pgm.h>
#include <unistd.h>
#include <assert.h>

#include "stdint.hpp"

namespace zmq
{
    // Sender engine has to use 3 fds
    enum {pgm_sender_fds = 3};
    
    // Receiver engine has to use 2 fds
    enum {pgm_receiver_fds = 2};

    // Indexes as fds are returned from pgm_transport_poll_info
    enum {pgm_recv_fd_idx = 0, pgm_wait_fd_idx = 1, pgm_send_fd_idx = 2};

    //  Encapsulates PGM socket
    class pgm_socket_t
    {
    public:
        // If receiver_ is true PGM transport is not generating SPM packets
        // in a case of passive_ receiver no NAKs are generated
        pgm_socket_t (bool receiver_, bool passive_, const char *network_, uint16_t port_, size_t readbuf_size_ = 0);

        //  Closes the transport
        ~pgm_socket_t ();

        // Returns number of FDs used by the PGM transport for (EPOLLIN or EPOLLOUT)
        int get_fd_count (short events_);

        // Fills pollfd structure
        int get_pfds (pollfd *fds_, int count_, short events_);

        // Read one byte from transport->waititng_pipe
        //char read_one_byte_from_waiting_pipe ();

        // Drop superuser privil
        void drop_superuser ();

        // Send one PGM data packet, transmit window owned memory.
        size_t write_one_pkt (unsigned char *data_, size_t data_len_);

        // Allocates one slice for packet in tx window (or returns from GTrashStack)
        unsigned char *alloc_one_pkt (bool can_fragment_);

        // Fees one slice allocated with alloc_one_pkt
        void free_one_pkt (unsigned char *data_, bool can_fragment_);

        // Returns max tsdu size
        size_t get_max_tsdu (bool can_fragment_);

        // Returns maximum count of apdus which fills readbuf_size_
        size_t get_max_apdu_at_once (size_t readbuf_size_);

        //
        size_t read_one_pkt (iovec *iov_);

        // reads iov_len apdus
        size_t read_pkt (iovec *iov_, size_t iov_len_);

    protected:
        pgm_transport_t* g_transport;

    private:
        // array of structure to store received data
        pgm_msgv_t msgv;

        // array of structure to store received data
        pgm_msgv_t *pgm_msgv;

        // number of pgm_msgv_t in pgm_msgv array
        size_t pgm_msgv_len;
    };
}
#endif
