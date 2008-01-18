/* zmq/kernel.hpp.  Generated from kernel.hpp.in by configure.  */
/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __ZMQ_KERNEL_HPP_INCLUDED__
#define __ZMQ_KERNEL_HPP_INCLUDED__

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <algorithm>
#include <limits.h>

#include "err.hpp"
#include "ysocketpair.hpp"
#include "ypipe.hpp"
#include "tcp_socket.hpp"
#include "ysemaphore.hpp"
#include "wire.hpp"

#define ZMQ_RECV_CHUNK_SIZE 8192

namespace zmq
{

    //  Prototype for the message body deallocation functions.
    //  Deliberately defined in the way to comply with standard C free.
    typedef void (free_fn) (void *data);

    //  Kernel class implements core of the messaging engine
    class kernel_t
    {
    protected:

        //  Internal structure to hold the chunks of data or messages
        struct msg_t
        {
            unsigned char *data;
            size_t size;
            size_t offset;
            free_fn *ffn;
        };

    public:

        //  Initialises the kernel. 'listen', 'address' and 'port' arguments
        //  are forwarded to underlying socket (see socket.hpp for reference)
        //  The kernel is by default created in batch mode. In case you want
        //  to create it in the x-mode, set xmode argument to true.
        kernel_t (bool listen, const char *address, unsigned short port,
                bool xmode);

        //  Destroy the kernel object
        ~kernel_t ();

        //  Send a message containing 'data' of size 'size'. Function pointed to
        //  by ffn will be used to deallocate the data. If the data are stored
        //  in static buffer, use NULL to indicate that no deallocation is
        //  needed.
        bool send (void *data, size_t size, free_fn *ffn);

        //  Receives a message. If message is not immediately available, blocks
        //  until one arrives. Use function returned in 'ffn' to deallocate the
        //  received data chunk.
        bool receive (void **data, size_t *size, free_fn **ffn);

    protected:

        //  Constants for individual commands sent through the command pipe
        enum
        {
            command_stop = 1,
            command_resurrect = 2
        };

        //  Top-level routine for worker thread. 
        //  Delegates all the work to 'do_work' function.
        static void *worker_routine (void *arg);

        //  Flushes as much data from send buffer to the socket as possible.
        //  Returns true is all the data are flushed, false otherwise.
        bool flush_sendbuf ();

        //  Fills as much data to specified message object as immediately
        //  possible. The message object has to have the memory allocated
        //  in advance and 'size' and 'offset' members set as needed.
        //  Returns false if the other party have closed the socket, otherwise
        //  true.
        bool fill_msg (msg_t &msg);

        //  Main routine of the worker thread
        void do_work ();
        
        //  Determines whether the kernel is currently in x-mode (true) or
        //  batch mode (false).
        bool xmode;

        //  If true, kernel will work only in batch mode, never switching to
        //  x-mode.
        bool xmode_blocked;

        //  If true, the other party have closed the underlying socket.
        bool peer_dead;

        //  The pipe for communication between sender thread and working thread
        //  and two pointers to hold the list of items retrieved from the pipe.
        ypipe_t <msg_t> send_pipe;
        ypipe_t <msg_t>::item_t *sendbuf_first;
        ypipe_t <msg_t>::item_t *sendbuf_last;

        //  The pipe for communication between receiver thread and working
        //  thread and two pointers to hold the list of items retrieved
        //  from the pipe.
        ypipe_t <msg_t> receive_pipe;
        ypipe_t <msg_t>::item_t *recvbuf_first;
        ypipe_t <msg_t>::item_t *recvbuf_last;

        //  Semaphore used by receiver thread to wait for a message if none
        //  is available immediately.
        ysemaphore_t receive_sem;

        //  Pipe to pass commands from client thread to worker thread
        ysocketpair_t command_pipe;

        //  Underlying socket
        tcp_socket_t sock;

        //  Worker thread
        pthread_t worker;
    };

}

#endif
