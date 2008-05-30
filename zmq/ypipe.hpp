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

#ifndef __ZMQ_YPIPE_HPP_INCLUDED__
#define __ZMQ_YPIPE_HPP_INCLUDED__

#include "atomic_ptr.hpp"
#include "yqueue.hpp"

namespace zmq
{

    //  Lock-free and wait free queue implementation.
    //  Only a single thread can read from the pipe at any specific moment.
    //  Only a single thread can write to the pipe at any specific moment.
    //
    //  T is the type of the object in the queue.
    //  If the template parameter D is set to true, it is quaranteed that
    //  the pipe will die in a finite time (so that you can swich to some
    //  other task). If D is set to false, reading from the pipe may result
    //  in an infinite cycle (if the pipe is continuosly fed by new elements).
    //  N is granularity of the pipe (how many elements have to be inserted
    //  till actual memory allocation is required).

    template <typename T, bool D, int N> class ypipe_t
    {
    public:

        //  Initialises the pipe. If 'dead' is set to true, the pipe is
        //  created in 'dead' state.
        ypipe_t (bool dead_ = true)
        {
            stop = false;
            w = NULL;
            queue.push ();
            r = &queue.back ();
            c.set (dead_ ? NULL : &queue.back ());
        }

        //  Write an item to the pipe.  Don't flush it yet.
        bool write (const T &value_)
        {
            queue.back () = value_;
            if (!w)
                w = &queue.back ();
            queue.push ();
        }

        //  Flush the messages into the pipe. If pipe is in 'dead' state,
        //  function returns false. Otherwise it returns true. Writing an item
        //  to the pipe revives it in case it is dead.
        bool flush ()
        {
            //  If there are no messages to flush, do nothing
            if (!w)
                return true;

            if (c.cas (w, &queue.back ()) == w) {
                w = NULL;
                return true;
            }
            c.set (&queue.back ());
            w = NULL;
            return false;
        }

        //  Reads an item from the pipe. Returns false if there is no value
        //  available.
        bool read (T *value_)
        {
            //  The value was prefetched already. Return it. No need to sleep
            //  as there may be more values prefetched.
            if (&queue.front () != r) {
                 *value_ = queue.front ();
                 queue.pop ();
                 return true;
            }

            //  We are requested to stop because D is set to true
            if (stop) {
                stop = false;
                return false;
            }

            //  Now let us prefetch more values
            if (D) {
                r = c.xchg (NULL);
                stop = true;  //  Next attempt to prefetch will fail
            }
            else
                r = c.cas (&queue.front (), NULL);

            if (&queue.front () == r) {
                stop = false;
                return false;
            }

            //  There was at least one value prefetched, return it
            *value_ = queue.front ();
            queue.pop ();
            return true;
        }

    protected:

        yqueue_t <T, N> queue;
        T *w;
        T *r;
        atomic_ptr_t <T> c;
        bool stop;
    };

}

#endif
