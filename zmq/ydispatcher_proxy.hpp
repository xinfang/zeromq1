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

        ydispatcher_proxy_t (ydispatcher_t <T> *dispatcher_, int engine_id_) :
            dispatcher (dispatcher_),
            engine_id (engine_id_)
        {
            engine_count = dispatcher->get_engine_count ();
            assert (engine_id < engine_count);
            engines_alive = engine_count;

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
                readbufs [buf_nbr].alive = true;
            }
        }

        ~ydispatcher_proxy_t ()
        {
            for (int writebuf_nbr = 0; writebuf_nbr != engine_count;
                  writebuf_nbr ++) {
                writebuf_t &writebuf = writebufs [writebuf_nbr];
                while (writebuf.first) {
                    item_t *o = writebuf.first;
                    writebuf.first = o->next;
                    delete o;
                }
            }
            delete [] writebufs;

            for (int readbuf_nbr = 0; readbuf_nbr != engine_count;
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

        inline void set_signaler (i_signaler *signaler_)
        {
            dispatcher->set_signaler (engine_id, signaler_);
        }

        inline int get_engines_alive ()
        {
            return engines_alive;
        }

        //  This function signals that there are messages sent
        //  to the engine by itself
        inline bool get_self_signal ()
        {
            return writebufs [engine_id].first;
        }

        void write (int destination_engine_id_, const T &value_)
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

        inline void instant_write (int destination_engine_id_, const T &value_)
        {
            if (destination_engine_id_ == engine_id)
                write (destination_engine_id_, value_);
            else
                dispatcher->write (engine_id, destination_engine_id_, value_);
        }

        void flush ()
        {
            for (int engine_nbr = 0; engine_nbr != engine_count;
                  engine_nbr ++) {
                writebuf_t &writebuf = writebufs [engine_nbr];
                if (writebuf.first && engine_nbr != engine_id) {
                    dispatcher->write (engine_id, engine_nbr,
                        writebuf.first, writebuf.last);
                    writebuf.first = NULL;
                    writebuf.last = NULL;
                }
            }
        }

        //  The result says whether value was fetched (true) or not (false).
        //  It says *nothing* about whether source of the messages went asleep.
        //  For info about awake/sleeping sources use get_engines_alive method.
        bool read (int source_engine_id_, T *value_)
        {
            readbuf_t &buf = readbufs [source_engine_id_];

            if (!buf.alive)
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
                    buf.alive = false;
                    engines_alive --;
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

        inline void revive (int source_engine_id_)
        {
            assert (!readbufs [source_engine_id_].alive);
            readbufs [source_engine_id_].alive = true;
            engines_alive ++;
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

        int engine_count;
        int engine_id;
        ydispatcher_t <T> *dispatcher;
        int engines_alive;

        writebuf_t *writebufs;
        readbuf_t *readbufs;
    };

}

#endif
