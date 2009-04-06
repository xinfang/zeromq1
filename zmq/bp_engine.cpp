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

#include <poll.h>

#include "bp_engine.hpp"

zmq::bp_engine_t *zmq::bp_engine_t::create (poll_thread_t *thread_,
    tcp_socket_t *socket_, size_t writebuf_size_, size_t readbuf_size_,
    const char *local_object_)
{
    bp_engine_t *instance = new bp_engine_t (thread_, socket_,
        writebuf_size_, readbuf_size_, local_object_);
    assert (instance);

    return instance;
}

zmq::bp_engine_t::bp_engine_t (poll_thread_t *thread_, tcp_socket_t *socket_,
      size_t writebuf_size_, size_t readbuf_size_,
      const char *local_object_) :
    context (thread_),
    demux (false),
    writebuf_size (writebuf_size_),
    write_size (0),
    write_pos (0),
    readbuf_size (readbuf_size_),
    read_size (0),
    read_pos (0),
    encoder (&mux),
    decoder (&demux),
    pipe_cnt (0),
    socket (socket_),
    poller (NULL),
    handle (0),
    socket_error (false),
    local_object (local_object_)
{
    //  Allocate read and write buffers.
    writebuf = (unsigned char*) malloc (writebuf_size);
    assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    assert (readbuf);

    //  Register BP engine with the I/O thread.
    thread_->register_engine (this);
}

zmq::bp_engine_t::~bp_engine_t ()
{
    free (readbuf);
    free (writebuf);
    delete socket;
}

void zmq::bp_engine_t::set_poller (i_poller *poller_, int handle_)
{
    //  Store the callback.
    poller = poller_;
    handle = handle_;

    //  Initialise the poll handle.
    poller->set_fd (handle, socket->get_fd ());
}

bool zmq::bp_engine_t::in_event ()
{
    //  This variable determines whether processing incoming messages is
    //  stuck because of exceeded pipe limits.
    bool stuck = read_pos < read_size;

    //  If there's no data to process in the buffer, read new data.
    if (read_pos == read_size) {

        //  Read as much data as possible to the read buffer.
        read_size = socket->read (readbuf, readbuf_size);
        read_pos = 0;

        if (read_size == -1) {

            //  If the other party closed the connection, stop polling.
            //  TODO: handle the event more gracefully.
            poller->reset_pollin (handle);
            return false;
        }
    }

    //  If there's at least one unprocessed byte in the buffer, process it.
    if (read_pos < read_size) {

        //  Push the data to the decoder and adjust read position in the buffer.
        int  nbytes = decoder.write (readbuf + read_pos, read_size - read_pos);
        read_pos += nbytes;

         //  If processing was stuck and become unstuck start reading
         //  from the socket. If it was unstuck and became stuck, stop polling
         //  for new data.
         if (stuck) {
             if (read_pos == read_size)

                 //  TODO: Speculative read should be used at this place to
                 //  avoid excessive poll. However, it looks like this can
                 //  result in infinite cycle in some cases, virtually
                 //  preventing other engines' access to CPU. Fix it.
                 poller->set_pollin (handle);
         }
         else {
             if (read_pos < read_size) {
                 poller->reset_pollin (handle);
             }    
         }

        //  If at least one byte was processed, flush any messages decoder
        //  may have produced.
        if (nbytes > 0)
            demux.flush ();
    }

    return true;
}

bool zmq::bp_engine_t::out_event ()
{
    //  If write buffer is empty, try to read new data from the encoder.
    if (write_pos == write_size) {

        write_size = encoder.read (writebuf, writebuf_size);
        write_pos = 0;

        //  If there are no data to write stop polling for output.
        if (write_size != writebuf_size)
            poller->reset_pollout (handle);
    }

    //  If there are any data to write in write buffer, write as much as
    //  possible to the socket.
    if (write_pos < write_size) {
        int nbytes = (ssize_t) socket->write (writebuf + write_pos,
            write_size - write_pos);
        if (nbytes == -1) 
            return false;
        write_pos += nbytes;
    }
    return true;
}

bool zmq::bp_engine_t::close_event ()
{
    if (socket->is_reconnectable ()) {

        //  Clean up. First mark buffers as fully processed.
        write_pos = write_size;
        read_pos = read_size;

        //  Cause encoder & decoder to drop any half-processed messages.
        encoder.clear ();
        decoder.clear ();

        //  Reconnect to the remote host.
        socket->reconnect ();

        //  Publish our ID.
        socket->send_string (local_object);

        poller->set_fd (handle, socket->get_fd ());
        poller->set_pollin (handle);
        return true;
    }

    if (!socket_error) {
        socket_error = true;

        //  Report connection failure to the client.
        //  If there is no error handler, application crashes immediately.
        //  If the error handler returns false, it crashes as well.
        //  If error handler returns true, the error is ignored.       
        error_handler_t *eh = get_error_handler ();
        assert (eh);
        if (!eh (local_object.c_str ()))
            assert (false);

        //  Notify all our receivers that this engine is shutting down.
        demux.terminate_pipes ();

        //  Notify senders that this engine is shutting down.
        mux.terminate_pipes ();
    }

    return false;
}

void zmq::bp_engine_t::process_command (const engine_command_t &command_)
{
    switch (command_.type) {
    case engine_command_t::revive:

        //  Forward the revive command to the pipe.
        if (!socket_error)
            command_.args.revive.pipe->revive ();

        //  There is at least one engine that has messages ready. Try to do
        //  speculative write to the socket and thus avoid polling for POLLOUT.
        poller->speculative_write (handle);
        break;

    case engine_command_t::head:

        //  Forward pipe statistics to the appropriate pipe.
        if (!socket_error) {
            command_.args.head.pipe->set_head (command_.args.head.position);
            in_event ();
        }
        break;

    case engine_command_t::send_to:

        //  Start sending messages to a pipe.
        if (!socket_error) {

            demux.send_to (command_.args.send_to.pipe);

            //  If pipe limits are set, POLLIN may be turned off
            //  because there are no pipes to send messages to.
            if (pipe_cnt ++ == 0)
                poller->set_pollin (handle);
        }
        break;

    case engine_command_t::receive_from:

        //  Start receiving messages from a pipe.
        if (!socket_error) {
            mux.receive_from (command_.args.receive_from.pipe);
            poller->set_pollout (handle);

            if (pipe_cnt ++ == 0)
                poller->set_pollin (handle);
        }
        break;

    case engine_command_t::destroy_pipe:

        demux.destroy_pipe (command_.args.destroy_pipe.pipe);
        delete command_.args.destroy_pipe.pipe;
        break;

    default:
        assert (false);
    }
}
