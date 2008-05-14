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

zmq::bp_engine_t *zmq::bp_engine_t::create (poll_thread_t *thread_,
    bool listen_, const char *address_, uint16_t port_,
    size_t writebuf_size_, size_t readbuf_size_)
{
    bp_engine_t *instance = new bp_engine_t (
        thread_, listen_, address_, port_, writebuf_size_, readbuf_size_);
    assert (instance);

    return instance;
}

zmq::bp_engine_t *zmq::bp_engine_t::create (poll_thread_t *thread_,
    int socket_, size_t writebuf_size_, size_t readbuf_size_)
{
    bp_engine_t *instance = new bp_engine_t (
        thread_, socket_, writebuf_size_, readbuf_size_);
    assert (instance);

    return instance;
}

zmq::bp_engine_t::bp_engine_t (poll_thread_t *thread_,
      bool listen_, const char *address_, uint16_t port_,
      size_t writebuf_size_, size_t readbuf_size_) :
    context (thread_),
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
    thread_->register_engine (this);
}

zmq::bp_engine_t::bp_engine_t (poll_thread_t *thread_, int socket_,
      size_t writebuf_size_, size_t readbuf_size_) :
    context (thread_),
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
    thread_->register_engine (this);
}

zmq::bp_engine_t::~bp_engine_t ()
{
    free (readbuf);
    free (writebuf);
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

        //  There is at least one engine that has messages ready -
        //  start polling the socket for writing.
        events |= POLLOUT;
        break;

    case engine_command_t::send_to:

        //  Start sending messages to a pipe
        demux.send_to (
            command_.args.send_to.pipe);
        break;

    case engine_command_t::receive_from:

        //  Start receiving messages from a pipe
        mux.receive_from (command_.args.receive_from.pipe);
        events |= POLLOUT;
        break;

    default:
        assert (false);
    }
}
