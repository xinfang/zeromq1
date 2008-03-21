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

#ifndef __ZMQ_DISPATCHER_HPP_INCLUDED__
#define __ZMQ_DISPATCHER_HPP_INCLUDED__

#include <vector>
#include <pthread.h>

#include "i_signaler.hpp"
#include "ypipe.hpp"
#include "cmsg.hpp"

namespace zmq
{

    //  Dispatcher implements bidirectional thread-safe ultra-efficient
    //  passing of objects between N threads.
    //
    //  It's designed to encapsulate all the cross-cutting functionality
    //  between two different threads. Namely, it consists of a pipe to pass
    //  messages and signaler to wake up receiver thread from sender thread
    //  when new messages are available.
    //
    //  Note that dispatcher is inefficient for passing messages within a thread
    //  (sender thread = receiver thread) as it performs unneccessary atomic
    //  operations. However, given that within-thread message transfer
    //  is not a cross-cutting functionality, the optimisation is not part
    //  of the class and should be implemented by individual threads.

    //  TODO: message bodies stored in the dispatcher are not deallocated
    //  when dispatcher is destroyed

    class dispatcher_t
    {
    public:

        typedef ypipe_t <cmsg_t>::item_t item_t;

        //  Create the dispatcher object
        dispatcher_t (int engine_count_);

        //  Destroy the dispatcher object
        ~dispatcher_t ();

        //  Returns number of engines dispatcher is preconfigured for
        inline int get_engine_count ()
        {
            return engine_count;
        }

        //  Registers receiver engine with the dispatcher.
        void set_signaler (int engine_id_, i_signaler *signaler_);

        //  Write a single message to diaptcher
        inline void write (int source_engine_id_, int destination_engine_id_,
            const cmsg_t &value_)
        {
            cell_t &cell = cells [source_engine_id_ * engine_count +
                destination_engine_id_];
            if (!cell.pipe.write (value_))
                cell.signaler->signal (source_engine_id_);
        }

        //  Write a message sequenct to dispatcher. 'first' parameter points
        //  to the first message in the sequence, 'last' parameter points to
        //  the last message in the sequence.
        inline void write (int source_engine_id_, int destination_engine_id_,
            item_t *first_, item_t *last_)
        {
            cell_t &cell = cells [source_engine_id_ * engine_count +
                destination_engine_id_];
            if (!cell.pipe.write (first_, last_))
                cell.signaler->signal (source_engine_id_);
        }

        //  Read message sequence from the dispatcher. 'first' parameter points
        //  to the first message in the sequence, 'last' parameter points to 
        //  one past the last message in the sequence.
        inline bool read (int source_engine_id_, int destination_engine_id_,
            item_t **first_, item_t **last_)
        {
            return cells [source_engine_id_ * engine_count +
                destination_engine_id_].pipe.read (first_, last_);
        }

        //  Assign an engine ID to the caller
        int allocate_engine_id ();

        //  Return engine ID to the pool of free engine IDs
        void deallocate_engine_id (int engine_id_);

    private:

        struct cell_t
        {
            ypipe_t <cmsg_t> pipe;
            i_signaler *signaler;
        };

        int engine_count;
        cell_t *cells;

        //  Vector specifying which engine IDs are used and which are not.
        //  The access to the vector is synchronised using mutex - this is OK
        //  as the performance of engine ID assignment is not critical for
        //  the performance of the system as a whole.
        std::vector <bool> used;
        pthread_mutex_t mutex;
    };

}

#endif

