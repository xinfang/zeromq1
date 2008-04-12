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
#include "command.hpp"

namespace zmq
{

    //  Dispatcher implements bidirectional thread-safe ultra-efficient
    //  passing of commands between N threads.
    //
    //  It's designed to encapsulate all the cross-cutting functionality
    //  between two different threads. Namely, it consists of a pipe to pass
    //  commands and signaler to wake up receiver thread when new commands
    //  are available.
    //
    //  Note that dispatcher is inefficient for passing messages within a thread
    //  (sender thread = receiver thread) as it performs unneccessary atomic
    //  operations. However, given that within-thread message transfer
    //  is not a cross-cutting functionality, the optimisation is not part
    //  of the class and should be implemented by individual threads.

    class dispatcher_t
    {
    public:

        typedef ypipe_t <command_t, true>::item_t item_t;

        //  Create the dispatcher object
        dispatcher_t (int thread_count_);

        //  Destroy the dispatcher object
        ~dispatcher_t ();

        //  Returns number of threads dispatcher is preconfigured for
        inline int get_thread_count ()
        {
            return thread_count;
        }

        //  Write a single message to diaptcher
        inline void write (int source_thread_id_, int destination_thread_id_,
            const command_t &value_)
        {
            if (!pipes [source_thread_id_ * thread_count +
                destination_thread_id_].write (value_))
                signalers [destination_thread_id_]->signal (source_thread_id_);
        }

        //  Write a message sequenct to dispatcher. 'first' parameter points
        //  to the first message in the sequence, 'last' parameter points to
        //  the last message in the sequence.
        inline void write (int source_thread_id_, int destination_thread_id_,
            item_t *first_, item_t *last_)
        {
            if (!pipes [source_thread_id_ * thread_count +
                destination_thread_id_].write (first_, last_))
                signalers [destination_thread_id_]->signal (source_thread_id_);
        }

        //  Read message sequence from the dispatcher. 'first' parameter points
        //  to the first message in the sequence, 'last' parameter points to 
        //  one past the last message in the sequence.
        inline bool read (int source_thread_id_, int destination_thread_id_,
            item_t **first_, item_t **last_)
        {
            return pipes [source_thread_id_ * thread_count +
                destination_thread_id_].read (first_, last_);
        }

        //  Assign an thread ID to the caller
        //  Regiter the supplied signaler with the thread
        int allocate_thread_id (i_signaler *signaler_);

        //  Return thread ID to the pool of free thread IDs
        void deallocate_thread_id (int thread_id_);

    private:

        int thread_count;
        ypipe_t <command_t, true> *pipes;
        std::vector <i_signaler*> signalers;

        //  Vector specifying which thread IDs are used and which are not.
        //  The access to the vector is synchronised using mutex - this is OK
        //  as the performance of thread ID assignment is not critical for
        //  the performance of the system as a whole.
        std::vector <bool> used;
        pthread_mutex_t mutex;
    };

}

#endif

