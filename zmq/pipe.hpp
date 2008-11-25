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
#include "raw_message.hpp"
#include "config.hpp"

namespace zmq
{
    class pipe_t
    {
    public:

        //  Initialise the pipe.
        pipe_t (struct i_context *source_context_,
            struct i_engine *source_engine_,
            struct i_context *destination_context_,
            struct i_engine *destination_engine_,
            int hwm_ = 0, int lwm_ = 0, int notification_period_ = 0);
        ~pipe_t ();

        //  Sets pointer to the mux object on receiving side of the pipe.
        void set_mux (class mux_t *mux_);

        //  Check whether message can be written to the pipe (i.e. whether
        //  pipe limits are exceeded. If true, it's OK to write the message
        //  to the pipe.
        bool check_write ();

        //  Write a message to the pipe. You should call check_write beforehand
        //  to determine whether pipe limits are exceeded.
        void write (raw_message_t *msg_);

        //  Write pipe delimiter to the pipe.
        void write_delimiter ();

        //  Flush all the written messages to be accessible for reading.
        void flush ();

        //  Returns true, if pipe delimiter was already received.
        inline bool eop ()
        {
            return endofpipe;
        }

        //  Reads a message from the pipe. Returns false if there is no message
        //  available.
        bool read (raw_message_t *msg);

        //  Make the dead pipe alive once more.
        void revive ();

        //  Process the 'head' command from reader thread.
        void set_head (uint64_t position_);

        //  Process the 'tail' command from writer thread.
        void set_tail (uint64_t position_);

        //  Notify the other end of the pipe that pipe is to be destroyed.
        void send_destroy_pipe ();

    private:

        //  The message pipe itself.
        typedef ypipe_t <raw_message_t, false, message_pipe_granularity>
            underlying_pipe_t;
        underlying_pipe_t pipe;

        //  Identification of the engine sending the messages to the pipe.
        i_context *source_context;
        i_engine *source_engine;

        //  Identification of the engine receiving the messages from the pipe.
        i_context *destination_context;
        i_engine *destination_engine;

        //  Pointer to the mux object on  the reading side of the pipe.
        mux_t *mux;

        //  If true we can read messages from the underlying ypipe.
        bool alive; 

        //  True if we've already read the pipe delimiter from
        //  the underlying pipe.
        bool endofpipe;

        //  If hwm is non-zero, the size of pipe is limited. In that case hwm
        //  is the high water mark for the pipe and lwm is the low water mark.
        int hwm;
        int lwm;

        //  How often should be tail position sent to the reader.
        //  Zero means never.
        int notification_period;

        //  Following message sequence numbers use RFC1982-like wraparound.

        //  Reader thread uses this variable to track the sequence number of
        //  the current message to read.
        uint64_t head;

        //  Writer thread uses 'tail' variable to track the sequence number of
        //  the current message to write.
        uint64_t tail;

        //  Writer thread keeps last head position reported by reader thread
        //  in this varaible.
        uint64_t last_head;

        //  Reader thread keeps last tail position reported by writer thread
        //  in this variable.
        uint64_t last_tail;

        pipe_t (const pipe_t&);
        void operator = (const pipe_t&);
    }; 

}

#endif
