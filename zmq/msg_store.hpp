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

#ifndef __ZMQ_MSG_STORE_HPP_INCLUDED__
#define __ZMQ_MSG_STORE_HPP_INCLUDED__

#include <sys/types.h>
#include <zmq/raw_message.hpp>

namespace zmq
{

    //  This class implements a persistent message store. Messages are
    //  retrieved from the store in the same order in which they were
    //  originally added.

    class msg_store_t
    {
    public:

        enum { default_block_size = 8192 };

        //  Initializes new message store.
        msg_store_t (const char *filename,
            size_t filesize, size_t block_size = default_block_size);

        ~msg_store_t ();

        //  Stores the message into the message store. The function
        //  returns false if the message store is full and true otherwise.
        bool store (raw_message_t *msg);

        //  Fetches the oldest message from the message store. It is an error
        //  call this function when the message store is empty.
        void fetch (raw_message_t *msg);

        //  Returns true if the message store is empty and false otherwise.
        bool empty ();

        //  Returns the number of messages kept in the message store.
        unsigned long size ();

    private:

        //  Copies data from the memory buffer to the message store's file.
        //  Wraps around when reaching maximum file size.
        void copy_from_file (void *buf, size_t count);

        //  Copies data from the message store's file to the memory buffer.
        //  Wraps around when reaching end-of-file.
        void copy_to_file (const void *buf, size_t count);

        //  Returns the buffer space available.
        size_t buffer_space ();

        void fill_read_buf ();

        void save_write_buf ();

        //  File descriptor to the message store's backing file.
        int fd;

        //  Name of the message store's backing file.
        std::string filename;

        //  Maximum size of the backing file.
        size_t filesize;

        //  File offset associated with the fd file descriptor.
        off_t file_pos;

        //  File offset the next message will be stored at.
        off_t write_pos;

        //  File offset the next message will be read from.
        off_t read_pos;

        //  Current number of messages kept in the message store.
        unsigned long n_msgs;

        size_t block_size;

        char *buf1;
        char *buf2;
        char *read_buf;
        char *write_buf;

        off_t write_buf_start_addr;
    };

}

#endif
