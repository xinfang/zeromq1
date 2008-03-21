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

#include "dispatcher_proxy.hpp"

zmq::dispatcher_proxy_t::dispatcher_proxy_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    engine_id = dispatcher->allocate_engine_id ();
    engine_count = dispatcher->get_engine_count ();
    pollable_count = 0;

    writebufs = new writebuf_t [engine_count];
    assert (writebufs);
    for (int buf_nbr = 0; buf_nbr != engine_count; buf_nbr ++) {
        writebufs [buf_nbr].first = NULL;
        writebufs [buf_nbr].last = NULL;
    }

    readbufs = new readbuf_t [engine_count];
    assert (readbufs);
    for (int buf_nbr = 0; buf_nbr != engine_count; buf_nbr ++) {
        readbufs [buf_nbr].first = NULL;
        readbufs [buf_nbr].last = NULL;
        readbufs [buf_nbr].pollable = false;
    }
}

zmq::dispatcher_proxy_t::~dispatcher_proxy_t ()
{
    for (int writebuf_nbr = 0; writebuf_nbr != engine_count; writebuf_nbr ++) {
        writebuf_t &writebuf = writebufs [writebuf_nbr];
        while (writebuf.first) {
            item_t *o = writebuf.first;
            writebuf.first = o->next;
            delete o;
        }
    }
    delete [] writebufs;

    for (int readbuf_nbr = 0; readbuf_nbr != engine_count; readbuf_nbr ++) {
        readbuf_t &readbuf = readbufs [readbuf_nbr];
        while (readbuf.first != readbuf.last) {
            item_t *o = readbuf.first;
            readbuf.first = o->next;
            delete o;
        }
    }
    delete [] readbufs;

    dispatcher->deallocate_engine_id (engine_id);
}

void zmq::dispatcher_proxy_t::write (int destination_engine_id_,
      const cmsg_t &value_)
{
    item_t *n = new item_t;
    assert (n);
    n->value = value_;
    n->next = NULL;

    writebuf_t &buf = writebufs [destination_engine_id_];
    if (buf.last)
        buf.last->next = n;
    buf.last = n;
    if (!buf.first)
        buf.first = n;
}

void zmq::dispatcher_proxy_t::flush ()
{
    for (int engine_nbr = 0; engine_nbr != engine_count; engine_nbr ++) {
        writebuf_t &writebuf = writebufs [engine_nbr];
        if (writebuf.first && engine_nbr != engine_id) {
            dispatcher->write (engine_id, engine_nbr,
                writebuf.first, writebuf.last);
            writebuf.first = NULL;
            writebuf.last = NULL;
        }
    }
}

bool zmq::dispatcher_proxy_t::read (int source_engine_id_, cmsg_t *value_)
{
    readbuf_t &buf = readbufs [source_engine_id_];

    if (buf.pollable)
        return false;

    if (buf.first != buf.last) {
        *value_ = buf.first->value;
        item_t *o = buf.first;
        buf.first = buf.first->next;
        delete o;
        return true;
    }

    if (source_engine_id_ == engine_id) {
        writebuf_t &writebuf = writebufs [source_engine_id_];
        if (!writebuf.first)
            return false;
        buf.first = writebuf.first;
        buf.last = NULL;
        writebuf.first = NULL;
        writebuf.last = NULL;
    }
    else {
        if (!dispatcher->read (source_engine_id_, engine_id,
              &buf.first, &buf.last)) {
            buf.pollable = true;
            pollable_count ++;
            return false;
        }
    }

    assert (buf.first != buf.last);
    *value_ = buf.first->value;
    item_t *o = buf.first;
    buf.first = buf.first->next;
    delete o;
    return true;
}

