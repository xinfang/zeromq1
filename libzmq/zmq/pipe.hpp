/*
    Copyright (c) 2007-2008 FastMQ Inc.

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

#ifndef __ZMQ_PIPE_HPP_INCLUDED__
#define __ZMQ_PIPE_HPP_INCLUDED__

#include <zmq/export.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/i_engine.hpp>
#include <zmq/ypipe.hpp>
#include <zmq/raw_message.hpp>
#include <zmq/config.hpp>

namespace zmq
{

    //  Pipe is a class to used for onw-way message transfer between two
    //  engines. Underlying transport mechanism is ypipe_t. Pipe itself
    //  adds support for communication with 0MQ engines.

    class pipe_t
    {
    public:

        //  Initialise the pipe.
        ZMQ_EXPORT pipe_t (struct i_thread *source_thread_,
            struct i_engine *source_engine_,
            struct i_thread *destination_thread_,
            struct i_engine *destination_engine_);
        ZMQ_EXPORT ~pipe_t ();

        //  Write a message to the pipe.
        inline void write (raw_message_t *msg_)
        {
            pipe.write (*msg_);
        }

        //  Flush all the written messages to be accessible for reading.
        ZMQ_EXPORT void flush ();

        //  Reads a message from the pipe.
        ZMQ_EXPORT bool read (raw_message_t *msg);

        //  Make the dead pipe alive once more.
        ZMQ_EXPORT void revive ();

        //  Used by the pipe writer to initialise pipe shut down.
        ZMQ_EXPORT void terminate_writer ();

        //  Confirms pipe shut down to the writer.
        ZMQ_EXPORT void writer_terminated ();

        //  Used by the pipe reader to initialise  pipe shut down.
        ZMQ_EXPORT void terminate_reader ();

        //  Confirms pipe shut down to the reader.
        ZMQ_EXPORT void reader_terminated ();

    private:

        //  The message pipe itself.
        typedef ypipe_t <raw_message_t, false, message_pipe_granularity>
            underlying_pipe_t;
        underlying_pipe_t pipe;

        //  Identification of the engine sending the messages to the pipe.
        i_thread *source_thread;
        i_engine *source_engine;

        //  Identification of the engine receiving the messages from the pipe.
        i_thread *destination_thread;
        i_engine *destination_engine;

        //  If true we can read messages from the underlying ypipe.
        bool alive; 

        //  Determines whether writer & reader side of the pipe are in the
        //  process of shutting down.
        bool writer_terminating;
        bool reader_terminating;

        pipe_t (const pipe_t&);
        void operator = (const pipe_t&);
    }; 

}

#endif
