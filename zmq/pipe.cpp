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

#include "pipe.hpp"
#include "command.hpp"

zmq::pipe_t::pipe_t (struct i_thread *source_thread_,
      int destination_thread_id_, struct i_pollable *destination_engine_) :
    pipe (false),
    writebuf_first (NULL),
    writebuf_last (NULL),
    thread (source_thread_),
    thread_id (destination_thread_id_),
    engine (destination_engine_),
    readbuf_first (NULL),
    readbuf_last (NULL),
    alive (true)
{
}

zmq::pipe_t::~pipe_t ()
{
    //  Destroy the write buffer
    while (writebuf_first != writebuf_last) {
        ypipe_t <void*, false>::item_t *o = writebuf_first;
        msg_dealloc (writebuf_first->value);
        writebuf_first = writebuf_first->next;
        delete o;
    }

    //  Destroy the read buffer
    while (readbuf_first != readbuf_last) {
        ypipe_t <void*, false>::item_t *o = readbuf_first;
        msg_dealloc (readbuf_first->value);
        readbuf_first = readbuf_first->next;
        delete o;
    }

    //  Destroy the messages in the pipe itself
    ypipe_t <void*, false>::item_t *first;
    ypipe_t <void*, false>::item_t *last;
    pipe.read (&first, &last);
    while (first != last) {
        ypipe_t <void*, false>::item_t *o = first;
        msg_dealloc (first->value);
        first = first->next;
        delete o;
    }
}

void zmq::pipe_t::instant_write (void *msg_)
{
    assert (writebuf_first == writebuf_last);
    if (!pipe.write (msg_))
        send_revive ();
}

void zmq::pipe_t::write (void *msg_)
{
    //  TODO: if there's only one message in the writebuf, we can
    //  avoid one alloc/dealloc pair by holding the pointer in a member variable

    ypipe_t <void*, false>::item_t *n = new ypipe_t <void*, false>::item_t;
    n->value = msg_;
    n->next = NULL;
    if (writebuf_last) {
        writebuf_last->next = n;
        writebuf_last = n;
    }
    else {
        writebuf_first = n;
        writebuf_last = n;
    }
}

void zmq::pipe_t::flush ()
{
    if (!writebuf_first)
        return;
    if (!pipe.write (writebuf_first, writebuf_last))
        send_revive ();
    writebuf_first = NULL;
    writebuf_last = NULL;
}

void zmq::pipe_t::revive ()
{
    assert (!alive);
    alive = true;
}

void zmq::pipe_t::send_revive ()
{
    command_t cmd;
    cmd.init_engine_revive (engine, this);
    thread->send_command (thread_id, cmd);
}

void *zmq::pipe_t::read ()
{
    //  If there are no messages in the read buffer...
    if (readbuf_first == readbuf_last) {

        //  If the pipe is dead, there's nothing we can do
        if (!alive)
            return NULL;

        //  Get new message batch from the pipe to the read buffer
        if (!pipe.read (&readbuf_first, &readbuf_last)) {
            alive = false;
            return NULL;
        }
    }

    //  Get the first message from the read buffer
    ypipe_t <void*, false>::item_t *o = readbuf_first;
    void *msg = o->value;
    readbuf_first = readbuf_first->next;
    delete o;
    return msg;
}

            
