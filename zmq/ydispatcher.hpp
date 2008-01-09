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

#ifndef __ZMQ_YDISPATCHER_HPP_INCLUDED__
#define __ZMQ_YDISPATCHER_HPP_INCLUDED__

#include <stddef.h>
#include <assert.h>

#include "i_signaler.hpp"
#include "ypipe.hpp"

namespace zmq
{

    //  Dispatcher implements bidirectional thread-safe ultra-efficient
    //  passing of objects between N threads.
    //
    //  It's designed to encapsulate all the cross-cutting functionality
    //  between two different threads. Namely, it's the pipe to pass messages
    //  and signaler to wake up receiver thread from sender thread.
    //
    //  Note that dispatcher is inefficient for passing messages within a thread
    //  (sender thread = receiver thread) as it performs unneccessary atomic
    //  operations. However, given that within-thread message transfer
    //  is not a cross-cutting functionality, the optimisation is not part
    //  of the class and should be implemented by individual threads.

    template <typename T> class ydispatcher_t
    {
    public:

        typedef typename ypipe_t <T>::item_t item_t;

        ydispatcher_t (int thread_count) :
            thread_count (thread_count)
        {
            cells = new cell_t [thread_count * thread_count];
            assert (cells);
            for (int cell_nbr = 0; cell_nbr != thread_count * thread_count;
                  cell_nbr ++)
                cells [cell_nbr].signaler = NULL;
        }

        ~ydispatcher_t ()
        {
            delete [] cells;
        }

        inline int get_thread_count ()
        {
            return thread_count;
        }

        //  Registers receiver thread with the dispatcher. In the future
        //  versions thread-id should be assigned by dispatcher, however,
        //  at the moment routing is done on thread-ids rather than on
        //  business criteria, therefore thread-id should be user-assigned.
        void set_signaler (int thread_id, i_signaler *signaler)
        {
            for (int thread_nbr = 0; thread_nbr != thread_count; thread_nbr ++)
                cells [thread_nbr * thread_count + thread_id].signaler =
                    signaler; 
        }

        inline void write (int sender_thread_id, int receiver_thread_id,
            const T &value)
        {
            cell_t &cell = cells [sender_thread_id * thread_count +
                receiver_thread_id];
            if (!cell.pipe.write (value))
                cell.signaler->signal (sender_thread_id);
        }

        inline void write (int sender_thread_id, int receiver_thread_id,
            item_t *first, item_t *last)
        {
            cell_t &cell = cells [sender_thread_id * thread_count +
                receiver_thread_id];
            if (!cell.pipe.write (first, last))
                cell.signaler->signal (sender_thread_id);
        }


        inline bool read (int sender_thread_id, int receiver_thread_id,
            item_t **first, item_t **last)
        {
            return cells [sender_thread_id * thread_count + receiver_thread_id].
                pipe.read (first, last);
        }

    protected:
        struct cell_t
        {
            ypipe_t <T> pipe;
            i_signaler *signaler;
        };

        int thread_count;
        cell_t *cells;
    };

}

#endif
