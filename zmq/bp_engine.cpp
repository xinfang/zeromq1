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

#include "bp_engine.hpp"

zmq::bp_engine_t::bp_engine_t (bool listen_, const char *address_,
      uint16_t port_, int source_thread_id_, int destination_thread_id_,
      size_t writebuf_size_, size_t readbuf_size_) :
    encoder (NULL),
    decoder (NULL),
    socket (listen_, address_, port_),
    writebuf_size (writebuf_size_),
    readbuf_size (readbuf_size_),
    write_size (0),
    write_pos (0),
    proxy (NULL),
    source_thread_id (source_thread_id_),
    destination_thread_id (destination_thread_id_)
{
    writebuf = (unsigned char*) malloc (writebuf_size);
    assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    assert (readbuf);
}

zmq::bp_engine_t::~bp_engine_t ()
{
    if (encoder)
        delete encoder;
    if (decoder)
        delete decoder;

    free (readbuf);
    free (writebuf);
}

void zmq::bp_engine_t::set_dispatcher_proxy (dispatcher_proxy_t *proxy_)
{
    proxy = proxy_;
    encoder = new bp_encoder_t (proxy, source_thread_id);
    assert (encoder);
    decoder = new bp_decoder_t (proxy, destination_thread_id);
    assert (decoder);
}

int zmq::bp_engine_t::get_fd ()
{
    return socket.get_fd ();
}

bool zmq::bp_engine_t::get_in ()
{
    return true;
}

bool zmq::bp_engine_t::get_out ()
{
    return true;
}

bool zmq::bp_engine_t::in_event ()
{
    size_t nbytes = socket.read (readbuf, readbuf_size);
    if (!nbytes)
        return false;
    decoder->write (readbuf, nbytes);
    proxy->flush ();
    return true;
}

bool zmq::bp_engine_t::out_event ()
{
    bool res = true;
    if (write_pos == write_size) {
    write_size = encoder->read (writebuf, writebuf_size);
    if (write_size < writebuf_size)
        res = false;
        write_pos = 0;
    }
    if (write_pos < write_size) {
        size_t nbytes = socket.write (writebuf + write_pos,
            write_size - write_pos);
        write_pos += nbytes;
    }
    return res;
}
