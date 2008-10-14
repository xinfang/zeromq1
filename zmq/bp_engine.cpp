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


#include "bp_engine.hpp"

zmq::bp_engine_t *zmq::bp_engine_t::create (i_thread *calling_thread_,
    i_thread *thread_, const char *hostname_, size_t writebuf_size_,
    size_t readbuf_size_, const char *local_object_)
{
    bp_engine_t *instance = new bp_engine_t (calling_thread_,
        thread_, hostname_, writebuf_size_, readbuf_size_, local_object_);
    assert (instance);

    return instance;
}

zmq::bp_engine_t *zmq::bp_engine_t::create (i_thread *calling_thread_,
    i_thread *thread_, tcp_listener_t &listener_, size_t writebuf_size_,
    size_t readbuf_size_, const char *local_object_)
{
    bp_engine_t *instance = new bp_engine_t (calling_thread_,
        thread_, listener_, writebuf_size_, readbuf_size_, local_object_);
    assert (instance);

    return instance;
}

zmq::bp_engine_t::bp_engine_t (i_thread *calling_thread_, i_thread *thread_,
      const char *hostname_, size_t writebuf_size_, size_t readbuf_size_,
      const char *local_object_) :
    writebuf_size (writebuf_size_),
    readbuf_size (readbuf_size_),
    write_size (0),
    write_pos (0),
    encoder (&mux),
    decoder (&demux),
    socket (hostname_),
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
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_engine_t::bp_engine_t (i_thread *calling_thread_, i_thread *thread_,
      tcp_listener_t &listener_, size_t writebuf_size_, size_t readbuf_size_,
      const char *local_object_) :
    writebuf_size (writebuf_size_),
    readbuf_size (readbuf_size_),
    write_size (0),
    write_pos (0),
    encoder (&mux),
    decoder (&demux),
    socket (listener_),
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
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);
}

zmq::bp_engine_t::~bp_engine_t ()
{
    free (readbuf);
    free (writebuf);
}

void zmq::bp_engine_t::set_poller (i_poller *poller_, int handle_)
{
    //  Store the callback.
    poller = poller_;
    handle = handle_;

    //  Initialise the poll handle.
    poller->set_fd (handle, socket.get_fd ());
    poller->set_pollin (handle);
}

bool zmq::bp_engine_t::in_event ()
{
    //  Read as much data as possible to the read buffer.
    size_t nbytes = socket.read (readbuf, readbuf_size);

    if (!nbytes) {

        //  If the other party closed the connection, stop polling.
        //  TODO: handle the event more gracefully
        poller->reset_pollin (handle);
        return false;
    }

    //  Push the data to the decoder
    decoder.write (readbuf, nbytes);

    //  Flush any messages decoder may have produced.
    demux.flush ();

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
        ssize_t nbytes = (ssize_t) socket.write (writebuf + write_pos,
            write_size - write_pos);
        if (nbytes <= 0) 
            return false;
        write_pos += nbytes;
    }
    return true;
}

void zmq::bp_engine_t::close_event ()
{
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

    case engine_command_t::send_to:

        //  Start sending messages to a pipe.
        if (!socket_error)
            demux.send_to (command_.args.send_to.pipe);
        break;

    case engine_command_t::receive_from:

        //  Start receiving messages from a pipe.
        if (!socket_error) {
            mux.receive_from (command_.args.receive_from.pipe);
            poller->set_pollout (handle);
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
