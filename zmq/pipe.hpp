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
#include "i_pollable.hpp"
#include "ypipe.hpp"
#include "msg.hpp"

namespace zmq
{

    class pipe_t
    {
    public:

        pipe_t (struct i_context *source_context_,
            struct i_pollable *source_engine_,
            struct i_context *destination_context_,
            struct i_pollable *destination_engine_);
        ~pipe_t ();

        void instant_write (void *msg_);
        void write (void *msg_);
        void flush ();
        void *read ();
        void revive ();

    private:

        void send_revive ();

        //  The message pipe itself
        ypipe_t <void*, false> pipe;

        //  Identification of the engine sendinf the messages to the pipe
        i_context *source_context;
        i_pollable *source_engine;

        //  Identification of the engine receiving the messages from the pipe
        i_context *destination_context;
        i_pollable *destination_engine;

        //  These variables should be accessed only by the methods called
        //  from the writing thread.
        ypipe_t <void*, false>::item_t *writebuf_first;
        ypipe_t <void*, false>::item_t *writebuf_last;

        //  These variables should be accessed only by the methods called
        //  from the reading thread.
        ypipe_t <void*, false>::item_t *readbuf_first;
        ypipe_t <void*, false>::item_t *readbuf_last;
        bool alive; 
    }; 

}

#endif
