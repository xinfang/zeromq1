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

#include "i_thread.hpp"
#include "i_engine.hpp"
#include "ypipe.hpp"
#include "raw_message.hpp"
#include "config.hpp"
#include "declspec_export.hpp"

namespace zmq
{
    class pipe_t
    {
    public:

        //  Initialise the pipe.
        declspec_export pipe_t (struct i_thread *source_thread_,
            struct i_engine *source_engine_,
            struct i_thread *destination_thread_,
            struct i_engine *destination_engine_);
        declspec_export ~pipe_t ();

        //  Write a message to the pipe.
        declspec_export inline void write (raw_message_t *msg_)
        {
            pipe.write (*msg_);
        }

        //  Write pipe delimiter to the pipe.
        declspec_export inline void write_delimiter ()
        {
            raw_message_t delimiter;
            raw_message_init_delimiter (&delimiter);
            pipe.write (delimiter);
            flush ();
        }

        //  Flush all the written messages to be accessible for reading.
        declspec_export inline void flush ()
        {
            if (!pipe.flush ())
                send_revive ();
        }

        //  Returns true, if pipe delimiter was already received.
        declspec_export bool eop ()
        {
            return endofpipe;
        }

        //  Reads a message from the pipe.
        declspec_export bool read (raw_message_t *msg);

        //  Make the dead pipe alive once more.
        declspec_export void revive ();

        //  Notify the other end of the pipe that pipe is to be destroyed.
        declspec_export void send_destroy_pipe ();

    private:

        declspec_export void send_revive ();

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

        //  True if we've already read the pipe delimiter from
        //  the underlying pipe.
        bool endofpipe;

        pipe_t (const pipe_t&);
        void operator = (const pipe_t&);
    }; 

}

#endif
