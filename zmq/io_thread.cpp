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

#include "io_thread.hpp"
#include "err.hpp"

zmq::io_thread_t::io_thread_t (dispatcher_t *dispatcher_, int thread_id_,
      int source_thread_id_, int destination_thread_id_, i_socket *socket_,
      size_t write_buffer_size_, size_t read_buffer_size_) :
    proxy (dispatcher_, thread_id_, &signaler),
    encoder (&proxy, source_thread_id_, write_buffer_size),
    decoder (&proxy, destination_thread_id_),
    socket (socket_),
    write_buffer_size (write_buffer_size_),
    read_buffer_size (read_buffer_size_),
    out_buf (NULL),
    out_buf_size (0),
    out_buf_pos (0)
{
    int rc = pthread_create (&worker, NULL, worker_routine, this);
    errno_assert (rc == 0);
}

zmq::io_thread_t::~io_thread_t ()
{
    signaler.signal (stop_event);

    int rc = pthread_join (worker, NULL);
    errno_assert (rc == 0);

    if (out_buf)
        free (out_buf);
}

void *zmq::io_thread_t::worker_routine (void *arg_)
{
    io_thread_t *self = (io_thread_t*) arg_;
    self->loop ();
    return 0;
}

void zmq::io_thread_t::loop ()
{
    pollfd pfd [2];
    pfd [0].fd = signaler.get_fd ();
    pfd [0].events = POLLIN;
    pfd [1].fd = socket->get_fd ();
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
                if (events [event] == stop_event)
                    return;
                proxy.revive (events [event]);
                pfd [1].events |= POLLOUT;
            }
        }

        if (pfd [1].revents & POLLOUT) {

            if (out_buf_pos == out_buf_size) {
                if (out_buf)
                    free (out_buf);
                if (!encoder.read (&out_buf, &out_buf_size))
                    pfd [1].events ^= POLLOUT;
                out_buf_pos = 0;
            }
            if (out_buf_pos < out_buf_size) {
                size_t nbytes = socket->write (out_buf + out_buf_pos,
                    out_buf_size - out_buf_pos);
                out_buf_pos += nbytes;
            }
        }

        if (pfd [1].revents & POLLIN) {

            unsigned char *buf = (unsigned char*) malloc (read_buffer_size);
            assert (buf);
            size_t nbytes = socket->read (buf, read_buffer_size);
            if (nbytes == 0) {
                free (buf);
                return;
            }
            decoder.write (buf, nbytes, free);
        }
    }
}
