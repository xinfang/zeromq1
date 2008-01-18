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
#include "err.hpp"

namespace zmq
{

    class io_thread_t 
    {
    public:

        io_thread_t (bool listen_, const char *address_, unsigned short port_,
              int thread_id_, dispatcher_t *dispatcher_, int source_thread_id_,
              int destination_thread_id_) :
            proxy (dispatcher_, thread_id_, &signaler),
            encoder (&proxy, 8192, source_thread_id_),
            decoder (&proxy, destination_thread_id_),
            socket (listen_, address_, port_),
            out_buf (NULL),
            out_buf_size (0),
            out_buf_pos (0)
        {
            int rc = pthread_create (&worker, NULL, worker_routine, this);
            errno_assert (rc == 0);
        }

        ~io_thread_t ()
        {
            int rc = pthread_join (worker, NULL);
            errno_assert (rc == 0);

            if (out_buf)
               free (out_buf);
        }

    protected:

        static void *worker_routine (void *arg_)
        {
            io_thread_t *self = (io_thread_t*) arg_;
            self->loop ();
            return 0;
        }

        void loop ()
        {
            pollfd pfd [2];
            pfd [0].fd = signaler.get_fd ();
            pfd [0].events = POLLIN;
            pfd [1].fd = socket.get_fd ();
            pfd [1].events = POLLIN | POLLOUT;

            while (true)
            {
                int rc = poll (pfd, 2, -1);
                errno_assert (rc != -1);
                assert (!(pfd [0].revents & (POLLERR | POLLHUP | POLLNVAL)));
                assert (!(pfd [1].revents & (POLLERR | POLLHUP | POLLNVAL)));

                if (pfd [0].revents & POLLIN) {
                    unsigned char events [256];
                    ssize_t nbytes = recv (pfd [0].fd, events, 256,
                        MSG_DONTWAIT);
                    errno_assert (nbytes != -1);
                    for (int event = 0; event != nbytes; event ++) {
                        proxy.revive (events [event]);
                        pfd [1].events |= POLLOUT;
printf ("pollout enabled\n");
                    }
                }

                if (pfd [1].revents & POLLOUT) {
printf ("got into pollout\n");
                    if (out_buf_pos == out_buf_size) {
                        if (out_buf)
                            free (out_buf);
                        if (!encoder.read (&out_buf, &out_buf_size))
                            pfd [1].events ^= POLLOUT;
                        out_buf_pos = 0;
                    }
                    if (out_buf_pos < out_buf_size) {
                        ssize_t nbytes = send (pfd [1].fd,
                            out_buf + out_buf_pos, out_buf_size - out_buf_pos,
                            MSG_DONTWAIT);
printf ("%d bytes send", (int) nbytes);
                        out_buf_pos += nbytes;
                    }
                }

                if (pfd [1].revents & POLLIN) {

                    unsigned char *buf = (unsigned char*) malloc (8192);
                    assert (buf);
                    ssize_t nbytes = recv (pfd [1].fd, buf, 8192, MSG_DONTWAIT);
                    errno_assert (nbytes != -1);
                    decoder.write (buf, nbytes, free);
                }
            }
        }

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
