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

#include <poll.h>

#include "bp_engine.hpp"

zmq::bp_engine_t::bp_engine_t (dispatcher_t *dispatcher_, int thread_id_,
      bool listen_, const char *address_, uint16_t port_,
      int source_thread_id_, int destination_thread_id_,
      size_t writebuf_size_, size_t readbuf_size_) :
    proxy (dispatcher_, thread_id_),
    encoder (&proxy, source_thread_id_),
    decoder (&proxy, destination_thread_id_),
    socket (listen_, address_, port_),
    events (POLLIN | POLLOUT),
    writebuf_size (writebuf_size_),
    readbuf_size (readbuf_size_),
    write_size (0),
    write_pos (0)
{
    writebuf = (unsigned char*) malloc (writebuf_size);
    assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    assert (readbuf);
}

zmq::bp_engine_t::~bp_engine_t ()
{
    free (readbuf);
    free (writebuf);
}

void zmq::bp_engine_t::set_signaler (i_signaler *signaler_)
{
    proxy.set_signaler (signaler_);
}

int zmq::bp_engine_t::get_fd ()
{
    return socket.get_fd ();
}

short zmq::bp_engine_t::get_events ()
{
    return events;
}

void zmq::bp_engine_t::revive (int thread_id_)
{
    proxy.revive (thread_id_);
    events |= POLLOUT;
}

void zmq::bp_engine_t::in_event ()
{
    size_t nbytes = socket.read (readbuf, readbuf_size);
    if (!nbytes) {
        events ^= POLLIN;
        return;
    }
    decoder.write (readbuf, nbytes);
    proxy.flush ();
}

void zmq::bp_engine_t::out_event ()
{
    if (write_pos == write_size) {
    write_size = encoder.read (writebuf, writebuf_size);
    if (write_size < writebuf_size)
        events ^= POLLOUT;
        write_pos = 0;
    }
    if (write_pos < write_size) {
        size_t nbytes = socket.write (writebuf + write_pos,
            write_size - write_pos);
        write_pos += nbytes;
    }
}
