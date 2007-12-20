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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <algorithm>

#include "err.hpp"
#include "spipe.hpp"
#include "ypipe.hpp"
#include "socket.hpp"
#include "ysemaphore.hpp"
#include "wire.hpp"

#define ZMQ_RECV_CHUNK_SIZE 8192
#define ZMQ_SEND_VECTOR_SIZE 1024

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
                bool xmode) :
            xmode (xmode),
            xmode_blocked (!xmode),
            peer_dead (false),
            send_pipe (xmode),
            receive_pipe (false),
            sock (listen, address, port)
        {
            //  Initialise sender and receiver buffers
            sendbuf_first = NULL;
            sendbuf_last = NULL;
            recvbuf_first = NULL;
            recvbuf_last = NULL;

            //  Start the worker thread
            int rc = pthread_create (&worker, NULL, worker_routine, (void*) this);
            errno_assert (rc == 0);
        }

        //  Destroy the kernel object
        ~kernel_t ()
        {
            //  Ask worker thread to stop
            command_pipe.write (command_stop);

            //  Wait for worker thread to terminate
            int rc = pthread_join (worker, NULL);
            errno_assert (rc == 0);

            //  Deallocate the items in the send buffer
            while (sendbuf_first != sendbuf_last) {
                ypipe_t <msg_t>::item_t *o = sendbuf_first;
                sendbuf_first = sendbuf_first->next;
                delete o;
            }

            //  Deallocate the items in the receive buffer
            while (recvbuf_first != recvbuf_last) {
                ypipe_t <msg_t>::item_t *o = recvbuf_first;
                recvbuf_first = recvbuf_first->next;
                delete o;
            }
        }

        //  Send a message containing 'data' of size 'size'. Function pointed to
        //  by ffn will be used to deallocate the data. If the data are stored
        //  in static buffer, use NULL to indicate that no deallocation is
        //  needed.
        bool send (void *data, size_t size, free_fn *ffn)
        {
            //  If the other party closed the socket, we are not able to
            //  send messages any more
            if (peer_dead)
                return false;

            msg_t msg = {(unsigned char*) data, size, 0, ffn};

            //  In batch mode, all the messages are simply enqueued into
            //  send pipe. If the pipe is dead, send 'resurrect' command to
            //  the worker thread.
            if (xmode_blocked)
            {
                if (!send_pipe.write (msg))
                    command_pipe.write (command_resurrect);
                return true;
            }

            //  If currenly not in x-mode, enqueue the messagge into send pipe.
            //  If the pipe is dead, switch into x-mode.
            if (!xmode) {
                if (send_pipe.write (msg, false))
                    return true;
                xmode = true;                
            }

            //  Send the message to the socket directly from the client's
            //  thread - the core of x-mode functionality.
            msghdr hdr;
            memset (&hdr, 0, sizeof (hdr));
            iovec iovs [2];
            hdr.msg_iov = iovs;
            unsigned char sizebuf [9];
            size_t header_size = msg.size < 255 ? 1 : 9;

            assert (msg.offset == 0);

            //  Fill in the iovec
            if (msg.size < 255) {
                sizebuf [0] = (unsigned char) msg.size;
                iovs [0].iov_base = &sizebuf;
                iovs [0].iov_len = 1;
            }
            else {
                sizebuf [0] = 0xff;         
                put_size_64 (sizebuf + 1, msg.size);
                iovs [0].iov_base = &sizebuf;
                iovs [0].iov_len = 9;
            }
            iovs [1].iov_base = msg.data;
            iovs [1].iov_len = msg.size;
            hdr.msg_iovlen = 2;

            //  Push the message to the socket
            ssize_t nbytes = sendmsg (sock, &hdr, MSG_DONTWAIT);
            errno_assert (nbytes > 0);

            //  If the send is successful, deallocate the message and
            //  remain in x-mode.
            msg.offset += nbytes;
            if (msg.offset == msg.size + header_size) {
                if (msg.ffn)
                    msg.ffn (msg.data);
                msg.data = NULL;
                msg.size = 0;
                return true;
            }

            //  If the message was written to the socket only partially,
            //  enqueue it into send pipe and resurrect the worker thread.
            //  We are actually switching to batch mode at this point.
            send_pipe.write (msg);
            command_pipe.write (command_resurrect);
            xmode = false;

            return true;
        }

        //  Receives a message. If message is not immediately available, blocks
        //  until one arrives. Use function returned in 'ffn' to deallocate the
        //  received data chunk.
        bool receive (void **data, size_t *size, free_fn **ffn)
        {
            //  If the other party have closed the socket, we cannot read
            //  messages.
            if (peer_dead)
                return false;

            //  The message to read
            msg_t msg = {NULL, 0, 0, NULL};

            //  Get and parse the message header
            unsigned char header_buf [9];
            msg_t hdr1 = {header_buf, 1, 0, NULL};
            if (!fill_msg (hdr1))
                return false;
            if (header_buf [0] != 0xff) {
                msg.size = (size_t) hdr1.data [0];
            }
            else {
                msg_t hdr2 = {header_buf + 1, 8, 0, NULL};
                if (!fill_msg (hdr2))
                    assert (0);
                msg.size = get_size_64 (hdr2.data);
            }

            //  Allocate the memory to store message body.
            msg.data = (unsigned char*) malloc (msg.size);
            assert (msg.data);

            if (!fill_msg (msg)) {
                free (msg.data);
                return false;
            }

            *data = msg.data;
            *size = msg.size;
            *ffn = msg.ffn;
            return true;
        }
    protected:

        //  Constants for individual commands sent through the command pipe
        enum
        {
            command_stop = 1,
            command_resurrect = 2
        };

        //  Top-level routine for worker thread. 
        //  Delegates all the work to 'do_work' function.
        static void *worker_routine (void *arg)
        {
            kernel_t *self = (kernel_t*) arg;
            self->do_work ();
            return NULL;
        }

        //  Flushes as much data from send buffer to the socket as possible.
        //  Returns true is all the data are flushed, false otherwise.
        bool flush_sendbuf ()
        {
            //  If send buffer is empty, there's nothing to do.
            if (sendbuf_first == sendbuf_last)
                return true;

            //  Initialise som structures needed for sending
            bool can_write = true;
            iovec iovs [ZMQ_SEND_VECTOR_SIZE];
            unsigned char sizes [ZMQ_SEND_VECTOR_SIZE * 9];
            msghdr hdr;
            memset (&hdr, 0, sizeof (hdr));
            hdr.msg_iov = iovs;

            //  Loop while socket is available for writing and there are still
            //  some data in the send buffer.
            while (can_write && sendbuf_first != sendbuf_last) {

                int pos = 0;
                size_t sizes_pos = 0;
                size_t sz = 0;
                ypipe_t <msg_t>::item_t *sendbuf_curr = sendbuf_first;

                //  Traverse the messages stored in send buffer and fill
                //  the iovec array in a corresponding way.
                while (sendbuf_curr != sendbuf_last) {

                    msg_t &msg = sendbuf_curr->value;
                    size_t header_size = msg.size < 255 ? 1 : 9;

                    if (msg.offset >= header_size) {

                        //  Store remainder of message body into an iovec.
                        iovs [pos].iov_base =
                            msg.data + msg.offset - header_size;
                        iovs [pos].iov_len =
                            msg.size - msg.offset + header_size;
                        sz += msg.size - msg.offset + header_size;
                        pos ++;
                    }
                    else {

                        //  Store message header into an iovec
                        if (msg.size < 255) {
                            sizes [sizes_pos] = (unsigned char) msg.size;
                            iovs [pos].iov_base = &(sizes [sizes_pos]);
                            iovs [pos].iov_len = 1;
                            sizes_pos ++;
                            sz ++;
                        }
                        else {
                            sizes [sizes_pos] = 0xff;
                            put_size_64 (sizes + sizes_pos + 1, msg.size);
                            iovs [pos].iov_base =
                                sizes + sizes_pos + msg.offset;
                            iovs [pos].iov_len = 9 - msg.offset;
                            sizes_pos += 9;
                            sz += 9 - msg.offset;
                        }
                        pos ++;

                        //  Store message body into an iovec
                        iovs [pos].iov_base = msg.data;
                        iovs [pos].iov_len = msg.size;
                        sz += msg.size;
                        pos ++;
                    }

                    //  Move to the next message in the send buffer
                    sendbuf_curr = sendbuf_curr->next;

                    //  If the iovec array have reached the OS-defined limit
                    //  stop generating new iovecs even if there are more data
                    //  available in the send buffer.
                    if (pos >= ZMQ_SEND_VECTOR_SIZE - 1)
                        break;
                }

                //  Physically push the iovec array into the socket.
                //  Note: The EAGAIN line is kind of strange, but the situation
                //  happens every now and then on Linux, so we have to handle
                //  it gracefully.
                hdr.msg_iovlen = pos;
                ssize_t nbytes = sendmsg (sock, &hdr, MSG_DONTWAIT);
                if (nbytes == -1 && errno == EAGAIN)
                    nbytes = 0;
                errno_assert (nbytes != -1);

                //  If not all the data was written, the socket is unavailable
                //  for the writing at the moment. We should stop writing, but
                //  still we have to adjust the offsets in the send buffer.
                if (nbytes < (ssize_t) sz)
                    can_write = false;

                //  Loop while all the written bytes are turned into offsets
                while (nbytes) {

                    //  How many bytes were yet to be sent?
                    size_t remaining = sendbuf_first->value.size -
                        sendbuf_first->value.offset +
                        (sendbuf_first->value.size >= 255 ? 9 : 1);

                    //  We have to adjust the offset of the last
                    //  partially writen message.
                    if (nbytes < (ssize_t) remaining) {
                        sendbuf_first->value.offset += nbytes;
                        return false;
                    }

                    //  We can deallocate completely written messages safely
                    assert (sendbuf_first != sendbuf_last);
                    ypipe_t <msg_t>::item_t *o = sendbuf_first;
                    sendbuf_first = sendbuf_first->next;
                    delete o;

                    nbytes -= remaining;
                }
            }

            return sendbuf_first == sendbuf_last;
        }

        //  Fills as much data to specified message object as immediately
        //  possible. The message object has to have the memory allocated
        //  in advance and 'size' and 'offset' members set as needed.
        //  Returns false if the other party have closed the socket, otherwise
        //  true.
        bool fill_msg (msg_t &msg)
        {
            assert (msg.offset < msg.size);

            //  Process the chunks in the receive buffer one by one
            while (true) {

                //  If there are no more chunks in the receive buffer, transfer
                //  all the chunks in the receive pipe to the receive buffer.
                //  If there is nothing in the pipe, wait for notification from 
                //  worker thread and transfer the chunks afterwards.
                if (recvbuf_first == recvbuf_last) {
                    if (!receive_pipe.read (&recvbuf_first, &recvbuf_last)) {
                        receive_sync.wait ();
                        if (!receive_pipe.read (&recvbuf_first, &recvbuf_last))
                            assert (0);
                    }

                    //  The empty chunk is sign of socket being closed be the
                    //  other party.
                    if (!recvbuf_first->value.data) {
                        peer_dead = true;
                        return false;
                    }
                }
                else {

                    //  Get the current chunk
                    msg_t *rbuf = &(recvbuf_first->value);

                    //  If there is no more data left in the chunk, move to the
                    //  next one and free the old one. If the next one is empty,
                    //  it is a sign of the socket being closed
                    //  by the other party.
                    if (rbuf->offset == rbuf->size) {
                        if (rbuf->ffn)
                            rbuf->ffn (rbuf->data);
                        ypipe_t <msg_t>::item_t *o = recvbuf_first;
                        recvbuf_first = recvbuf_first->next;
                        delete o;
                        if (recvbuf_first != recvbuf_last &&
                              !recvbuf_first->value.data) {
                            peer_dead = true;
                            return false;
                        }
                    }
                    else {

                        //  Transfer as much data from the current chunk to the
                        //  message as possible, alter the offsets
                        //  in both chunk and message.
                        size_t to_read = std::min (rbuf->size - rbuf->offset,
                            msg.size - msg.offset);
                        memcpy (msg.data + msg.offset,
                            rbuf->data + rbuf->offset, to_read);
                        rbuf->offset += to_read;
                        msg.offset += to_read;

                        //  If the whole message is already read, return
                        assert (msg.offset <= msg.size);
                        if (msg.size == msg.offset)
                            return true;
                    }
                }
            }
        }

        //  Main routine of the worker thread
        void do_work ()
        {
            //  Initialise structures needed for polling
            pollfd pfd [2];
            pfd [0].fd = command_pipe;
            pfd [0].events = POLLIN;
            pfd [1].fd = sock;
            pfd [1].events = 0;

            bool stopping = false;
            bool pollin = true;
            bool pollout = xmode_blocked;

            //  Main loop of the worker thread
            while (true) {

                //  If the worker thread was requested to stop and there are
                //  no more data to send, exit the main loop.
                if (stopping && !pollout)
                    break;

                //  Adjust the poll events according to 'pollin' and 'pollout'
                //  variables, start the polling and check the results
                //  for errors.
                pfd [1].events =
                    (pollin ? POLLIN : 0) | (pollout ? POLLOUT : 0);
                int rc = poll (pfd, 2, -1);
                errno_assert (rc != -1);
                assert ((pfd [0].revents &
                    (POLLERR | POLLHUP | POLLNVAL)) == 0);
                assert ((pfd [1].revents & POLLNVAL) == 0);

                //  Command from client thread was received by the worker
                //  thread.
                if (pfd [0].revents & POLLIN) {
                    switch (command_pipe.read ()) {
                    case command_stop:

                        //  Worker thread is scheduled to terminate.
                        stopping = true;
                        break;

                    case command_resurrect:

                        //  Worker thread starts polling for write availability
                        //  of the socket after new data to send are available
                        //  in the send pipe.
                        pollout = true;
                        break;
                    }
                }

                //  Socket is available for reading
                if (pfd [1].revents & POLLIN) {

                    //  Allocate a memory chunk and read as much data as
                    //  possible. Only one chunk is read at once to cease the
                    //  control to writing part of the loop periodically even
                    //  if there is input on the socket all the time.
                    unsigned char *chunk =
                        (unsigned char*) malloc (ZMQ_RECV_CHUNK_SIZE);
                    assert (chunk);
                    ssize_t nbytes = recv (sock, chunk, ZMQ_RECV_CHUNK_SIZE, 0);
                    errno_assert (nbytes >= 0);

                    //  Enqueue empty chunk to let the client thread know that
                    //  the other party have closed the socket.
                    if (nbytes == 0) {
                        pollin = false;
                        free (chunk);
                        chunk = NULL;
                    }

                    //  Push the chunk to the receiver pipe. If the pipe was
                    //  dead, post the semaphore to unblock client thread
                    //  that is waiting for a message.
                    msg_t ch = {chunk, nbytes, 0, free};
                    if (!receive_pipe.write (ch))
                        receive_sync.post ();

                    //  If batch mode is set by user and the received
                    //  chunk is too small, wait for a while to improve the
                    //  batching ratio. NB: This code needs a careful 
                    //  reconsideration in the future releases. (See
                    //  nanosleep documentation for description of lack of
                    //  precision of this function on Linux.)
                    if (xmode_blocked && nbytes < ZMQ_RECV_CHUNK_SIZE / 2) {
                        timespec tmspc = {0, 10000};
                        int rc = nanosleep (&tmspc, NULL);
                        errno_assert (rc == 0);
                    }
                }

                //  Socket is available for writing
                if (pfd [1].revents & POLLOUT) {

                    //  Flush the data remaining in the sendbuf to the socket.
                    //  If successful, get new data from send pipe to send
                    //  buffer. If none are available, stop polling for writing
                    //  availability of the socket. If there are data, try to
                    //  flush them to the socket and cease the control to other
                    //  tasks.
                    if (flush_sendbuf ()) {
                        if (!send_pipe.read (&sendbuf_first, &sendbuf_last)) {
                            pollout = false;
                        }
                        else
                            flush_sendbuf ();
                    }
                }
            }
        }

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
        ysemaphore_t receive_sync;

        //  Pipe to pass commands from client thread to worker thread
        spipe_t command_pipe;

        //  Underlying socket
        socket_t sock;

        //  Worker thread
        pthread_t worker;
    };

}

#endif
