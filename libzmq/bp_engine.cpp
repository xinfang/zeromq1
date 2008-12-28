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

#include <zmq/bp_engine.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/err.hpp>
#include <zmq/config.hpp>

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
    write_size (0),
    write_pos (0),
    readbuf_size (readbuf_size_),
    read_size (0),
    read_pos (0),
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
    write_size (0),
    write_pos (0),
    readbuf_size (readbuf_size_),
    read_size (0),
    read_pos (0),
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

void zmq::bp_engine_t::get_watermarks (int *hwm_, int *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
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
    //  This variable determines whether processing incoming messages is
    //  stuck because of exceeded pipe limits.
    bool stuck = read_pos < read_size;

    //  If there's no data to process in the buffer, read new data.
    if (read_pos == read_size) {

        //  Read as much data as possible to the read buffer.
        read_size = socket.read (readbuf, readbuf_size);
        read_pos = 0;

        //  The other party closed the connection.
        if (read_size == -1) {

            //  Report connection failure to the client.
            //  If there is no error handler, application crashes immediately.
            //  If the error handler returns false, it crashes as well.
            //  If error handler returns true, the error is ignored.       
            error_handler_t *eh = get_error_handler ();
            assert (eh);
            if (!eh (local_object.c_str ()))
                assert (false);

            //  Remove the file descriptor from the pollset.
            poller->rm_fd (handle);    

            //  Ask all inbound & outound pipes to shut down.
            demux.initialise_shutdown ();
            mux.initialise_shutdown ();
            shutting_down = true;
            return;
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

    case engine_command_t::head:

        //  Forward pipe statistics to the appropriate pipe.
        if (!shutting_down) {
            command_.args.head.pipe->set_head (command_.args.head.position);
            in_event ();
        }
        break;

    case engine_command_t::send_to:

        if (!shutting_down) {

            //  If pipe limits are set, POLLIN may be turned off
            //  because there are no pipes to send messages to.
            //  So, if this is the first pipe in demux, start polling.
            if (demux.no_pipes ())
                poller->set_pollin (handle);

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
