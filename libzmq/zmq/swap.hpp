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

#ifndef __ZMQ_SWAP_HPP_INCLUDED__
#define __ZMQ_SWAP_DAM_HPP_INCLUDED__

#include <zmq/platform.hpp>

#include <string>
#include <sys/types.h>

#include <zmq/raw_message.hpp>
#include <zmq/atomic_counter.hpp>

namespace zmq
{

    //  This class implements a swap. Messages are retrieved from
    //  the dam in the same order as they entered it.

    class swap_t
    {
    public:

        enum { default_block_size = 8192 };

        //  Initializes swap.
        swap_t (int64_t filesize_, size_t block_size_ = default_block_size);

        ~swap_t ();

        //  Stores the message into the swap. The function
        //  returns false if the swap is full and true otherwise.
        bool store (raw_message_t *msg_);

        //  Fetches the oldest message from the swap. It is an error
        //  to call this function when the swap is empty.
        void fetch (raw_message_t *msg_);

        //  Returns true if the swap is empty and false otherwise.
        bool empty ();

        //  Returns the number of messages kept in the swap.
        unsigned long size ();

    private:

        //  Class member used for naming swap files.
        static atomic_counter_t counter;

        //  Copies data from the memory buffer to the swap's file.
        //  Wraps around when reaching maximum file size.
        void copy_from_file (void *buffer_, size_t count_);

        //  Copies data from the swap's file to the memory buffer.
        //  Wraps around when reaching end-of-file.
        void copy_to_file (const void *buffer_, size_t count_);

        //  Returns the buffer space available.
        int64_t buffer_space ();

        void fill_read_buf ();

        void save_write_buf ();

        //  File descriptor to the swap's backing file.
        int fd;

        //  Name of the swap's backing file.
        std::string filename;

        //  Maximum size of the backing file.
        int64_t filesize;

        //  File offset associated with the fd file descriptor.
        int64_t file_pos;

        //  File offset the next message will be stored at.
        int64_t write_pos;

        //  File offset the next message will be read from.
        int64_t read_pos;

        //  Current number of messages kept in the swap.
        unsigned long n_msgs;

        size_t block_size;

        char *buf1;
        char *buf2;
        char *read_buf;
        char *write_buf;

        int64_t write_buf_start_addr;
    };

}

#endif
