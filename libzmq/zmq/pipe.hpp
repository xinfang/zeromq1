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

#ifndef __ZMQ_PIPE_HPP_INCLUDED__
#define __ZMQ_PIPE_HPP_INCLUDED__

#include <zmq/stdint.hpp>
#include <zmq/i_thread.hpp>
#include <zmq/ypipe.hpp>
#include <zmq/raw_message.hpp>
#include <zmq/config.hpp>
#include <zmq/swap.hpp>

namespace zmq
{

    //  Pipe is a class to used for onw-way message transfer between two
    //  engines. Underlying transport mechanism is ypipe_t. Pipe itself
    //  adds support for communication with 0MQ engines and storing
    //  messages in file based message store.

    class pipe_t
    {
    public:

        //  Initialise the pipe.
        pipe_t (struct i_thread *source_thread_,
            struct i_engine *source_engine_,
            struct i_thread *destination_thread_,
            struct i_engine *destination_engine_);
        ~pipe_t ();

        //  Check whether message can be written to the pipe (i.e. whether
        //  pipe limits are exceeded. If true, it's OK to write the message
        //  to the pipe.
        bool check_write (raw_message_t *msg_);

        //  Write a message to the pipe.
        void write (raw_message_t *msg_);

        //  Write gap notification to the pipe. This call cannot fail. If the
        //  pipe is full, the notification is delayed and placed ot the queue
        //  once it becomes available for writing. Several subsequent delayed
        //  gap notifications are merged into a single one. If written to the
        //  pipe notification is flushed instantly.
        void gap ();

        //  Flush all the written messages to be accessible for reading.
        void flush ();

        //  Reads a message from the pipe.
        bool read (raw_message_t *msg);

        //  Process the 'head' command from reader thread.
        void set_head (uint64_t position_);

        //  Sets mux index of the pipe. By adding 1 to the index in the
        //  internal representation we can use value of 0 for 'invalid
        //  index' tag.
        inline void set_index (size_t index_)
        {
            mux_index = index_ + 1;
        }

        //  Retrieves mux index of the pipe.
        inline size_t index ()
        {
            return mux_index - 1;
        }

        //  Wake up the message consumer.
        void revive_reader ();

        //  Inform the message producer about the number of messages
        //  consumed so far.
        void notify_writer (uint64_t position_);

        //  Used by the pipe writer to initialise pipe shut down.
        void terminate_writer ();

        //  Confirms pipe shut down to the writer.
        void writer_terminated ();

        //  Used by the pipe reader to initialise  pipe shut down.
        void terminate_reader ();

        //  Confirms pipe shut down to the reader.
        void reader_terminated ();

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

        //  Index of this pipe in the mux object containing it.
        //  It is redundant information, however, it allows the schedular to
        //  achieve O(1) complexity. Value of 0 means invalid index.
        size_t mux_index;

        //  If hwm is non-zero, the size of pipe is limited. In that case hwm
        //  is the high water mark for the pipe and lwm is the low water mark.
        int64_t hwm;
        int64_t lwm;

        //  Following message sequence numbers use RFC1982-like wraparound.

        //  Reader thread uses this variable to track the sequence number of
        //  the current message to read.
        uint64_t head;

        //  Writer thread keeps last head position reported by reader thread
        //  in this varaible.
        uint64_t last_head_position;

        //  If true, there's a gap notification delayed because the pipe
        //  was full.
        bool delayed_gap;

        //  Number of messages kept in main memory.
        uint64_t in_core_msg_cnt;

        //  Message store keeps messages when the memory buffer is full.
        swap_t *swap;

        //  Flag indicating whether the swapping has been activated or not.
        bool swapping;

        //  Number of messages kept in the swap file.
        size_t in_swap_msg_cnt;

        //  Refills the memory buffer from the swap file.
        void swap_in ();

        //  Determines whether writer & reader side of the pipe are in the
        //  process of shutting down.
        bool writer_terminating;
        bool reader_terminating;

        pipe_t (const pipe_t&);
        void operator = (const pipe_t&);

    }; 

}

#endif
