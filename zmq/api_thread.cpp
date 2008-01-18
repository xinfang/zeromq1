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

#include "api_thread.hpp"

zmq::api_thread_t::api_thread_t (int thread_id_, dispatcher_t *dispatcher_) :
    thread_id (thread_id_),
    dispatcher (dispatcher_)
{
    thread_count = dispatcher->get_thread_count ();
    threads_alive = thread_count;
    current_thread = thread_count - 1;
    assert (thread_id < thread_count);

    recvbufs = new recvbuf_t [thread_count];
    assert (recvbufs);
    for (int buf_nbr = 0; buf_nbr != thread_count; buf_nbr ++) {
        recvbufs [buf_nbr].alive = true;
        recvbufs [buf_nbr].first = NULL;
        recvbufs [buf_nbr].last = NULL;
    }

    dispatcher->set_signaler (thread_id, &pollset);
}

zmq::api_thread_t::~api_thread_t ()
{
    for (int thread_nbr = 0; thread_nbr != thread_count; thread_nbr ++)
        while (recvbufs [thread_nbr].first !=
              recvbufs [thread_nbr].last) {
            dispatcher_t::item_t *o = recvbufs [thread_nbr].first;
            recvbufs [thread_nbr].first = o->next;
            delete o;
        }
}

void zmq::api_thread_t::send (int destination_thread_id_, const cmsg_t &value_)
{
    dispatcher->write (thread_id, destination_thread_id_, value_);
}

void zmq::api_thread_t::receive (cmsg_t *value_)
{
    while (true) {

        current_thread = (current_thread + 1) % thread_count;

        if (current_thread == 0) {
            uint32_t signals;
            if (threads_alive)
                signals = pollset.check ();
            else
                signals = pollset.poll ();
            for (int thread_nbr = 0; thread_nbr != thread_count;
                  thread_nbr ++) {
                if (signals & 0x0001) {
                    if (!recvbufs [thread_nbr].alive)
                        threads_alive ++;
                    recvbufs [thread_nbr].alive = true;
                }
                signals >>= 1;
            }
        }

        if (recvbufs [current_thread].first ==
              recvbufs [current_thread].last &&
              recvbufs [current_thread].alive) {
            recvbufs [current_thread].alive = dispatcher->read (
                current_thread, thread_id,
                &recvbufs [current_thread].first,
                &recvbufs [current_thread].last);
            if (!recvbufs [current_thread].alive)
                threads_alive --;
        }

        if (recvbufs [current_thread].first !=
              recvbufs [current_thread].last) {
            dispatcher_t::item_t *o = recvbufs [current_thread].first;
            *value_ = o->value;
            recvbufs [current_thread].first = o->next;
            delete o;
            return;
        }       
    }
}

