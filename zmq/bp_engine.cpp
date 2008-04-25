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

zmq::bp_engine_t *zmq::bp_engine_t::create (bool listen_,
    const char *address_, uint16_t port_,
    size_t writebuf_size_, size_t readbuf_size_)
{
    bp_engine_t *instance = new bp_engine_t (
        listen_, address_, port_, writebuf_size_, readbuf_size_);
    assert (instance);
    return instance;
}

zmq::bp_engine_t *zmq::bp_engine_t::create (int socket_,
    size_t writebuf_size_, size_t readbuf_size_)
{
    bp_engine_t *instance = new bp_engine_t (
        socket_, writebuf_size_, readbuf_size_);
    assert (instance);
    return instance;
}

zmq::bp_engine_t::bp_engine_t (bool listen_, const char *address_,
      uint16_t port_, size_t writebuf_size_, size_t readbuf_size_) :
    thread (NULL),
    encoder (&mux),
    decoder (&demux),
    socket (listen_, address_, port_),
    events (POLLIN),
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

zmq::bp_engine_t::bp_engine_t (int socket_,
      size_t writebuf_size_, size_t readbuf_size_) :
    thread (NULL),
    encoder (&mux),
    decoder (&demux),
    socket (socket_),
    events (POLLIN),
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

void zmq::bp_engine_t::set_thread (i_thread *thread_)
{
    thread = thread_;
    thread->register_engine (this);
}

int zmq::bp_engine_t::get_fd ()
{
    return socket.get_fd ();
}

short zmq::bp_engine_t::get_events ()
{
    //  TODO
    //  return events | (proxy.has_messages () ? POLLOUT : 0);
    return events;
}

void zmq::bp_engine_t::in_event ()
{
    //  Read as much data as possible to the read buffer
    size_t nbytes = socket.read (readbuf, readbuf_size);

    if (!nbytes) {

        //  If the other party closed the connection, stop polling
        //  TODO: handle the event more gracefully
        events ^= POLLIN;
        return;
    }

    //  Push the data to the decoder
    decoder.write (readbuf, nbytes);

    //  Flush any messages decoder may have produced
    demux.flush ();
}

void zmq::bp_engine_t::out_event ()
{
    //  If write buffer is empty, try to read new data from the encoder
    if (write_pos == write_size) {

        write_size = encoder.read (writebuf, writebuf_size);
        write_pos = 0;

        //  If there are no data to write stop polling for output
        if (!write_size)
            events ^= POLLOUT;
    }

    //  If there are any data to write in write buffer, write as much as
    //  possible to the socket.
    if (write_pos < write_size) {
        size_t nbytes = socket.write (writebuf + write_pos,
            write_size - write_pos);
        write_pos += nbytes;
    }
}

void zmq::bp_engine_t::process_command (const engine_command_t &command_)
{
    switch (command_.type) {
    case engine_command_t::revive:

        //  Forward the revive command to the pipe
        command_.args.revive.pipe->revive ();

        //  There is at least one engine that has messages ready - start polling
        //  the socket for writing.
        events |= POLLOUT;
        break;

    default:
        assert (false);
    }
}
