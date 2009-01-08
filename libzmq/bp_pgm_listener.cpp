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

#include <zmq/platform.hpp>

#if defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX

#include <zmq/bp_pgm_listener.hpp>
#include <zmq/config.hpp>
#include <zmq/formatting.hpp>
#include <zmq/ip.hpp>
#include <iostream>

#define PGM_SENDER_DEBUG
#define PGM_SENDER_DEBUG_LEVEL 4

// level 1 = key behaviour
// level 2 = processing flow
// level 4 = infos

#ifndef PGM_SENDER_DEBUG
#   define zmq_log(n, ...)  while (0)
#else
#   define zmq_log(n, ...)    do { if ((n) <= PGM_SENDER_DEBUG_LEVEL) { printf (__VA_ARGS__);}} while (0)
#endif


zmq::bp_pgm_listener_t *zmq::bp_pgm_listener_t::create (
    i_thread *calling_thread_, i_thread *thread_, const char *interface_,
    int handler_thread_count_, i_thread **handler_threads_, bool source_,
    i_thread *peer_thread_, i_engine *peer_engine_,
    const char *peer_name_)
{
    bp_pgm_listener_t *instance = new bp_pgm_listener_t (calling_thread_,
        thread_, interface_, handler_thread_count_, handler_threads_, source_,
        peer_thread_, peer_engine_, peer_name_);
    assert (instance);

    return instance;
}

zmq::bp_pgm_listener_t::bp_pgm_listener_t (i_thread *calling_thread_,
      i_thread *thread_, const char *interface_, int handler_thread_count_,
      i_thread **handler_threads_, bool source_,
      i_thread *peer_thread_, i_engine *peer_engine_,
      const char *peer_name_) :
    source (source_),
    thread (thread_),
    peer_thread (peer_thread_),
    peer_engine (peer_engine_),
    encoder (&mux),
    epgm_socket (false, false, interface_),
    txw_slice (NULL),
    max_tsdu (0),
    write_size (0),
    write_pos (0), 
    first_message_offest (-1)

{
    //  Copy the peer name.
    zmq_strncpy (peer_name, peer_name_, sizeof (peer_name));
    peer_name [sizeof (peer_name) - 1] = '\0';

    //  Store interface.
    sprintf (arguments, "bp/pgm://%s", interface_);

    //  Initialise the array of threads to handle new connections.
    assert (handler_thread_count_ > 0);
    for (int thread_nbr = 0; thread_nbr != handler_thread_count_; thread_nbr ++)
        handler_threads.push_back (handler_threads_ [thread_nbr]);
    current_handler_thread = 0;

    // Get max tsdu size from transmit window, 
    // will be used as max size for filling buffer by encoder
    max_tsdu = epgm_socket.get_max_tsdu (false);

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_engine (this);
    calling_thread_->send_command (thread_, command);

    //  The newly created engine serves as a local destination of messages
    //  I.e. it sends messages received from the peer engine to the socket.
    i_thread *destination_thread =
        handler_threads [current_handler_thread];
    i_engine *destination_engine = this;

    //  Create the pipe to the newly created engine.
    pipe_t *pipe = new pipe_t (peer_thread, peer_engine,
        destination_thread, destination_engine);
    assert (pipe);

    //  Bind new engine to the destination end of the pipe.
    command_t cmd_receive_from;
    cmd_receive_from.init_engine_receive_from (
        destination_engine, "", pipe);
    thread->send_command (destination_thread, cmd_receive_from);

    //  Bind the peer to the source end of the pipe.
    command_t cmd_send_to;
    cmd_send_to.init_engine_send_to (peer_engine, peer_name, pipe);
    thread->send_command (peer_thread, cmd_send_to);
}

zmq::bp_pgm_listener_t::~bp_pgm_listener_t ()
{
    if (txw_slice) {
        zmq_log (2, "Freeing unused slice\n");
        epgm_socket.free_one_pkt (txw_slice, false);
    }
}

zmq::engine_type_t zmq::bp_pgm_listener_t::type ()
{
    return engine_type_fd;
}

void zmq::bp_pgm_listener_t::get_watermarks (uint64_t *hwm_, uint64_t *lwm_)
{
    *hwm_ = bp_hwm;
    *lwm_ = bp_lwm;
}

void zmq::bp_pgm_listener_t::register_event (i_poller *poller_)
{
    //  Store the callback.
    poller = poller_;

    //  Alocate 1 fd for PGM socket
    int socket_fd;

    //  Fill socket_fd from PGM transport
    epgm_socket.get_sender_fd (&socket_fd);

    //  Add socket_fd into poller.
    handle = poller->add_fd (socket_fd, this);

    zmq_log (4, "Got handle from poller.\n");

    //  Set POLLOUT for socket_handle
    poller->set_pollout (handle);
}

void zmq::bp_pgm_listener_t::in_event ()
{
    assert (false);
}

void zmq::bp_pgm_listener_t::out_event ()
{
    // POLLOUT event from send socket
    zmq_log (4, "Got POLLOUT from poller, %s(%i)\n", __FILE__, __LINE__);

    //  If write buffer is empty, try to read new data from the encoder
    if (write_pos == write_size) {
        // get memory slice from tx window if we do not have already one
        if (!txw_slice) {
            txw_slice = epgm_socket.alloc_one_pkt (false);
            zmq_log (2, "Alocated packet in tx window\n");
        }

        write_size = encoder.read (txw_slice + sizeof (uint16_t), 
            max_tsdu - sizeof (uint16_t), &first_message_offest);
        write_pos = 0;

        zmq_log (2, "read %iB from encoder offset %i, %s(%i)\n", 
            (int)write_size, (int)first_message_offest, __FILE__, __LINE__);

        //  If there are no data to write stop polling for output
        if (!write_size) {
            poller->reset_pollout (handle);
            zmq_log (2, "POLLOUT stopped, %s(%i)\n", __FILE__, __LINE__);
        } else {
            // Addning uint16_t for offset in a case when encoder returned > 0B
            write_size += sizeof (uint16_t);
        }
    }

    //  If there are any data to write in write buffer, write them into the socket
    //  note that all data has to written in one write
    if (write_pos < write_size) {
        size_t nbytes = epgm_socket.write_one_pkt_with_offset (txw_slice + write_pos,
            write_size - write_pos, first_message_offest);

        zmq_log (2, "wrote %iB/%iB, %s(%i)\n", (int)(write_size - write_pos), (int)nbytes, __FILE__, __LINE__);
               
        // we can write all packer or 0 which means rate limit reached
        if (write_size - write_pos != nbytes && nbytes != 0) {
            zmq_log (1, "write_size - write_pos %i, nbytes %i, %s(%i)",
                (int)(write_size - write_pos), (int)nbytes, __FILE__, __LINE__);
            assert (false);
        }

        if (!nbytes) {
            zmq_log (1, "pgm rate limit reached, %s(%i)\n", __FILE__, __LINE__);
        }

        // after sending data slice is owned by tx window
        if (nbytes) {
            txw_slice = NULL;
        }

        write_pos += nbytes;
    }
}

void zmq::bp_pgm_listener_t::unregister_event ()
{
    //  TODO: implement this
    assert (false);
}

void zmq::bp_pgm_listener_t::process_command (const engine_command_t &command_)
{
    switch (command_.type) {
        case engine_command_t::receive_from:
            //  Start receiving messages from a pipe.
            mux.receive_from (command_.args.receive_from.pipe, shutting_down);
            if (!shutting_down)
                poller->set_pollout (handle);
            break;

        case engine_command_t::revive:

            //  We have some messages in encoder
            if (!shutting_down) {
                
                //  Forward the revive command to the pipe. 
                command_.args.revive.pipe->revive ();
                
                poller->set_pollout (handle);
                out_event ();
            }
            break;

        default:
            std::cout << "command_type: " << command_.type << std::endl << std::flush;
            assert (false);
    }
}

const char *zmq::bp_pgm_listener_t::get_arguments ()
{
    return arguments;
}

#endif
