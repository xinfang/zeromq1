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

#include <zmq/platform.hpp>

#if ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <glib.h>
#include <pgm/pgm.h>

#include <zmq/stdint.hpp>

namespace zmq
{
    //  Encapsulates PGM socket.
    class pgm_socket_t
    {
    public:
        //  If receiver_ is true PGM transport is not generating SPM packets
        //  in a case of passive_ receiver no NAKs are generated.
        pgm_socket_t (bool receiver_, bool passive_, const char *interface_, 
            size_t readbuf_size_ = 0);

        //  Closes the transport.
        ~pgm_socket_t ();

        //   Get receiver fds. recv_fd is from transport->recv_sock
        //   waiting_pipe_fd is from transport->waiting_pipe [0]
        int get_receiver_fds (int *recv_fd_, int *waiting_pipe_fd_);

        //   Get sender fd. sender_fd is from transport->send_sock.
        int get_sender_fd (int *sender_fd_);

        //  Drop superuser privileges.
        void drop_superuser ();

        //  Send one APDU, transmit window owned memory.
        size_t write_one_pkt (unsigned char *data_, size_t data_len_);

        //  Allocates one slice for packet in tx window.
        unsigned char *alloc_one_pkt (bool can_fragment_);

        //  Fees one slice allocated with alloc_one_pkt.
        void free_one_pkt (unsigned char *data_, bool can_fragment_);

        //  Returns max tsdu size.
        size_t get_max_tsdu_size (bool can_fragment_);

        //  Returns maximum count of apdus which fills readbuf_size_
        size_t get_max_apdu_at_once (size_t readbuf_size_);

        //  Receive exactly iov_len count APDUs.
        size_t read_pkt (iovec *iov_, size_t iov_len_);

    protected:
    
        //  OpenPGM transport
        pgm_transport_t* g_transport;

        //  Sender transport uses 1 fd.
        enum {pgm_sender_fd_count = 1};
    
        //  Receiver transport uses 2 fd.
        enum {pgm_receiver_fd_count = 2};

    private:

        //  Array of pgm_msgv_t structures to store received data.
        pgm_msgv_t *pgm_msgv;

        //  Size of pgm_msgv array.
        size_t pgm_msgv_len;
    };
}
#endif

#endif
