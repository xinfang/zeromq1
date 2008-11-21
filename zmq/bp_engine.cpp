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
#include "dispatcher.hpp"
#include "err.hpp"

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
    local_object (local_object_),
    shutting_down (false)
{
    //  Allocate read and write buffers.
    writebuf = (unsigned char*) malloc (writebuf_size);
    errno_assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    errno_assert (readbuf);

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
    local_object (local_object_),
    shutting_down (false)
{
    //  Allocate read and write buffers.
    writebuf = (unsigned char*) malloc (writebuf_size);
    errno_assert (writebuf);
    readbuf = (unsigned char*) malloc (readbuf_size);
    errno_assert (readbuf);

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

zmq::engine_type_t zmq::bp_engine_t::type ()
{
    return engine_type_fd;
}

void zmq::bp_engine_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Initialise the poll handle.
    handle = poller->add_fd (socket.get_fd (), this);
    poller->set_pollin (handle);
}

void zmq::bp_engine_t::in_event ()
{
    //  Read as much data as possible to the read buffer.
    int nbytes = socket.read (readbuf, readbuf_size);

    //  The other party closed the connection.
    if (nbytes == -1) {
        //  Remove the file descriptor from the pollset.
        poller->rm_fd (handle);    

        //  Ask all inbound & outound pipes to shut down.
        demux.initialise_shutdown ();
        mux.initialise_shutdown ();
        shutting_down = true;
        return;
    }

    //  Push the data to the decoder
    decoder.write (readbuf, nbytes);

    //  Flush any messages decoder may have produced.
    demux.flush ();
}

void zmq::bp_engine_t::out_event ()
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
        int nbytes = socket.write (writebuf + write_pos,
            write_size - write_pos);

        //  The other party closed the connection.
        if (nbytes == -1) {
            return;
        }

        write_pos += nbytes;
    }
}

void zmq::bp_engine_t::unregister_event ()
{
    //  TODO: Implement this.
    assert (false);
}

void zmq::bp_engine_t::process_command (const engine_command_t &command_)
{
    switch (command_.type) {
    case engine_command_t::revive:

        if (!shutting_down) {

            //  Forward the revive command to the pipe.
            command_.args.revive.pipe->revive ();

            //  There is at least one engine that has messages ready. Try to
            //  write data to the socket, thus eliminating one polling
            //  for POLLOUT event.
            poller->set_pollout (handle);
            out_event ();
        }
        break;

    case engine_command_t::send_to:

        //  TODO:  'send_to' command should hold reference to the pipe -
        //         to be rewritten to avoid possible memory leak.
        if (!shutting_down) {

            //  Start sending messages to a pipe.
            demux.send_to (command_.args.send_to.pipe);
        }
        break;

    case engine_command_t::receive_from:

        //  Start receiving messages from a pipe.
        mux.receive_from (command_.args.receive_from.pipe, shutting_down);
        if (!shutting_down)
            poller->set_pollout (handle);
        break;

    case engine_command_t::terminate_pipe:

        //  Forward the command to the pipe.
        command_.args.terminate_pipe.pipe->writer_terminated ();

        //  Remove all references to the pipe.
        demux.release_pipe (command_.args.terminate_pipe.pipe);
        break;

    case engine_command_t::terminate_pipe_ack:

        //  Forward the command to the pipe.
        command_.args.terminate_pipe_ack.pipe->reader_terminated ();

        //  Remove all references to the pipe.
        mux.release_pipe (command_.args.terminate_pipe_ack.pipe);
        break;

    default:
        assert (false);
    }
}
