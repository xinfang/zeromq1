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

#ifndef __ZMQ_PIPE_HPP_INCLUDED__
#define __ZMQ_PIPE_HPP_INCLUDED__

#include "i_context.hpp"
#include "i_engine.hpp"
#include "ypipe.hpp"
#include "msg.hpp"
#include "config.hpp"

namespace zmq
{

    class pipe_t
    {
    public:

        pipe_t (struct i_context *source_context_,
            struct i_engine *source_engine_,
            struct i_context *destination_context_,
            struct i_engine *destination_engine_);
        ~pipe_t ();

        inline void write (void *msg_)
        {
            pipe.write (msg_);
        }

        inline void flush ()
        {
            if (!pipe.flush ())
                send_revive ();
        }

        bool eop ()
        {
            return endofpipe;
        }

        void *read ();
        void revive ();
        void send_destroy_pipe ();

    private:

        void send_revive ();

        //  The message pipe itself
        ypipe_t <void*, false, message_pipe_granularity> pipe;

        //  Identification of the engine sending the messages to the pipe
        i_context *source_context;
        i_engine *source_engine;

        //  Identification of the engine receiving the messages from the pipe
        i_context *destination_context;
        i_engine *destination_engine;

        bool alive; 
        bool endofpipe;
    }; 

}

#endif
