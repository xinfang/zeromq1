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

#ifndef __ZMQ_YDISPATCHER_PROXY_HPP_INCLUDED__
#define __ZMQ_YDISPATCHER_PROXY_HPP_INCLUDED__

#include <stddef.h>
#include <assert.h>

#include "ydispatcher.hpp"

namespace zmq
{

    template <typename T> class ydispatcher_proxy_t
    {
    public:

        typedef typename ydispatcher_t <T>::item_t item_t;

        ydispatcher_proxy_t (ydispatcher_t <T> *dispatcher_, int thread_id_,
              i_signaler *signaler_) :
            dispatcher (dispatcher_),
            thread_id (thread_id_)
        {
            thread_count = dispatcher->get_thread_count ();
            assert (thread_id < thread_count);
            threads_alive = thread_count;
            dispatcher->set_signaler (thread_id, signaler_);

            writebufs = new writebuf_t [thread_count];
            assert (writebufs);
            for (int buf_nbr = 0; buf_nbr != thread_count; buf_nbr ++) {
                writebufs [buf_nbr].first = NULL;
                writebufs [buf_nbr].last = NULL;
            }

            readbufs = new readbuf_t [thread_count];
            assert (readbufs);
            for (int buf_nbr = 0; buf_nbr != thread_count; buf_nbr ++) {
                readbufs [buf_nbr].first = NULL;
                readbufs [buf_nbr].last = NULL;
                readbufs [buf_nbr].alive = true;
            }
        }

        ~ydispatcher_proxy_t ()
        {
            for (int writebuf_nbr = 0; writebuf_nbr != thread_count;
                  writebuf_nbr ++) {
                writebuf_t &writebuf = writebufs [writebuf_nbr];
                while (writebuf.first) {
                    item_t *o = writebuf.first;
                    writebuf.first = o->next;
                    delete o;
                }
            }
            delete [] writebufs;

            for (int readbuf_nbr = 0; readbuf_nbr != thread_count;
                  readbuf_nbr ++) {
                readbuf_t &readbuf = readbufs [readbuf_nbr];
                while (readbuf.first != readbuf.last) {
                    item_t *o = readbuf.first;
                    readbuf.first = o->next;
                    delete o;
                }
            }
            delete [] readbufs;
        }

        inline int get_threads_alive ()
        {
            return threads_alive;
        }

        void write (int destination_thread_id_, const T &value_)
        {
             item_t *n = new item_t;
             assert (n);
             n->value = value_;
             n->next = NULL;

             writebuf_t &buf = writebufs [destination_thread_id_];
             if (buf.last)
                 buf.last->next = n;
             buf.last = n;
             if (!buf.first)
                 buf.first = n;
        }

        inline void instant_write (int destination_thread_id_, const T &value_)
        {
            if (destination_thread_id_ == thread_id)
                write (destination_thread_id_, value_);
            else
                dispatcher->write (thread_id, destination_thread_id_, value_);
        }

        void flush ()
        {
            for (int thread_nbr = 0; thread_nbr != thread_count;
                  thread_nbr ++) {
                writebuf_t &writebuf = writebufs [thread_nbr];
                if (writebuf.first && thread_nbr != thread_id) {
                    dispatcher->write (thread_id, thread_nbr,
                        writebuf.first, writebuf.last);
                    writebuf.first = NULL;
                    writebuf.last = NULL;
                }
            }
        }

        //  The result says whether value was fetched (true) or not (false).
        //  It says *nothing* about whether source of the messages went asleep.
        //  For info about awake/sleeping sources use get_threads_alive method.
        bool read (int source_thread_id_, T *value_)
        {
            readbuf_t &buf = readbufs [source_thread_id_];

            if (!buf.alive)
                return false;

            if (buf.first != buf.last) {
                *value_ = buf.first->value;
                item_t *o = buf.first;
                buf.first = buf.first->next;
                delete o;
                return true;
            }

            if (source_thread_id_ == thread_id) {
                writebuf_t &writebuf = writebufs [source_thread_id_];
                if (!writebuf.first)
                    return false;
                buf.first = writebuf.first;
                buf.last = NULL;
                writebuf.first = NULL;
                writebuf.last = NULL;
            }
            else {
                if (!dispatcher->read (source_thread_id_, thread_id,
                      &buf.first, &buf.last)) {
                    buf.alive = false;
                    threads_alive --;
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

        inline void revive (int source_thread_id_)
        {
            assert (!readbufs [source_thread_id_].alive);
            readbufs [source_thread_id_].alive = true;
            threads_alive ++;
        }

    protected:

        struct writebuf_t
        {
            item_t *first;
            item_t *last;
        };

        struct readbuf_t
        {
            item_t *first;
            item_t *last;
            bool alive;
        };

        int thread_count;
        int thread_id;
        ydispatcher_t <T> *dispatcher;
        int threads_alive;

        writebuf_t *writebufs;
        readbuf_t *readbufs;
    };

}

#endif