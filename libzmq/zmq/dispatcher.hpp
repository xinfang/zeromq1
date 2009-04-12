/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_DISPATCHER_HPP_INCLUDED__
#define __ZMQ_DISPATCHER_HPP_INCLUDED__

#include <vector>

#include <zmq/export.hpp>
#include <zmq/platform.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/i_signaler.hpp>
#include <zmq/ypipe.hpp>
#include <zmq/mutex.hpp>
#include <zmq/config.hpp>

namespace zmq
{

    //  Dispatcher implements bidirectional thread-safe ultra-efficient
    //  passing of commands between N threads.
    //
    //  It consists of a ypipes to pass commands and signalers to wake up
    //  the receiver thread when new commands are available.
    //
    //  Note that dispatcher is inefficient for passing messages within a thread
    //  (sender thread = receiver thread). The optimisation is not part
    //  of the class and should be implemented by individual threads
    //  (presumably by calling the command handling function directly).
    
    class dispatcher_t
    {
    public:

        //  Create the dispatcher object. The actual number of threads
        //  supported is determined by 'thread_count'.
        ZMQ_EXPORT dispatcher_t (int thread_count_);

        //  Destroy the dispatcher object.
        ZMQ_EXPORT ~dispatcher_t ();

        //  Returns number of threads dispatcher is preconfigured for.
        inline int get_thread_count ()
        {
            return thread_count;
        }

        //  Write command to the dispatcher.
        inline void write (int source_thread_id_, 
            int destination_thread_id_, const command_t &value_)
        {
            command_pipe_t &pipe = pipes [source_thread_id_ *
                  thread_count + destination_thread_id_];
            pipe.write (value_);
            if (!pipe.flush ())
                signalers [destination_thread_id_]->signal (source_thread_id_);
        }

        //  Read command from the dispatcher. Returns false if there is no
        //  command available.
        inline bool read (int source_thread_id_, 
            int destination_thread_id_, command_t *command_)
        {
            return pipes [source_thread_id_ * thread_count +
                destination_thread_id_].read (command_);
        }

        //  Assign an thread ID to the caller. Register the supplied signaler
        //  with the thread.
        ZMQ_EXPORT int allocate_thread_id (i_thread *thread_,
            i_signaler *signaler_);

    private:

        //  Pipe to hold the commands.
        typedef ypipe_t <command_t, true,
            command_pipe_granularity> command_pipe_t;

        //  Number of threads dispatcher is preconfigured for.
        int thread_count;

        //  NxN matrix of command pipes.
        command_pipe_t *pipes;

        //  Signalers to wake up individual threads.
        std::vector <i_signaler*> signalers;

        //  Threads to destroy on shutdown.
        std::vector <i_thread*> threads;

        //  Vector specifying which thread IDs are used and which are not.
        //  The access to the vector is synchronised using mutex - this is OK
        //  as the performance of thread ID assignment is not critical for
        //  the performance of the system as a whole.
        std::vector <bool> used;
        mutex_t sync;

        dispatcher_t (const dispatcher_t&);
        void operator = (const dispatcher_t&);
    };

    //  Prototype of the error handling function.
    typedef bool (error_handler_t) (const char *local_object_,
        const char *remote_object_);

    //  Global error handler.
    extern error_handler_t * volatile eh;

    //  Returns pointer to the current error handling function.
    error_handler_t * get_error_handler();
 
    //  Sets error function. Error function will be called when a connection
    //  breaks. Name of the object (exchange or queue) on the local side
    //  of the connection is passed to the function, so that client is able
    //  to determine which connection was broken. If error function returns
    //  true, disconnection is silently ignored. If it returns false,
    //  application will fail (this is the default behaviour).
    ZMQ_EXPORT void set_error_handler (error_handler_t *eh_);
    
}

#endif

