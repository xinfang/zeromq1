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

#include <zmq/bp_tcp_listener.hpp>
#include <zmq/bp_tcp_sender.hpp>
#include <zmq/bp_tcp_receiver.hpp>
#include <zmq/data_distributor.hpp>
#include <zmq/config.hpp>
#include <zmq/formatting.hpp>
#include <zmq/err.hpp>
#include <zmq/mux.hpp>

zmq::bp_tcp_listener_t::bp_tcp_listener_t (i_thread *thread_,
      const char *interface_, int handler_thread_count_,
      i_thread **handler_threads_, bool sender_,
      i_thread *peer_thread_, i_engine *peer_engine_,
      const char *peer_name_) :
    sender (sender_),
    poller (NULL),
    thread (thread_),
    peer_thread (peer_thread_),
    peer_engine (peer_engine_),
    listener (interface_)
{
    //  Copy the peer name.
    zmq_strncpy (peer_name, peer_name_, sizeof (peer_name));

    //  Initialise the array of threads to handle new connections.
    zmq_assert (handler_thread_count_ > 0);
    for (int thread_nbr = 0; thread_nbr != handler_thread_count_; thread_nbr ++)
        handler_threads.push_back (handler_threads_ [thread_nbr]);
    current_handler_thread = 0;
}

zmq::bp_tcp_listener_t::~bp_tcp_listener_t ()
{
}

void zmq::bp_tcp_listener_t::start (i_thread *current_thread_,
    i_thread *engine_thread_)
{
    zmq_assert (thread == engine_thread_);

    //  Register BP engine with the I/O thread.
    command_t command;
    command.init_register_pollable (this);
    current_thread_->send_command (engine_thread_, command);
}

zmq::i_demux *zmq::bp_tcp_listener_t::get_demux ()
{
    zmq_assert (false);
    return NULL;
}

zmq::i_mux *zmq::bp_tcp_listener_t::get_mux ()
{
    zmq_assert (false);
    return NULL;
}

void zmq::bp_tcp_listener_t::register_event (i_poller *poller_)
{
    poller = poller_;
    handle = poller->add_fd (listener.get_fd (), this);
    poller->set_pollin (handle);
}

void zmq::bp_tcp_listener_t::in_event ()
{
    fd_t fd = listener.accept ();
    if (fd == retired_fd)
        return;

    if (!sender) {
        //  Create demux for receiver engine.
        i_demux *demux = new data_distributor_t (bp_hwm, bp_lwm);

        //  Create the engine to take care of the connection.
        //  TODO: make buffer size configurable by user
        bp_tcp_receiver_t *engine = new bp_tcp_receiver_t (demux,
            fd, peer_name);
        zmq_assert (engine);
        engine->start (thread, handler_threads [current_handler_thread]);

        //  The newly created engine serves as a local source of messages
        //  I.e. it reads messages from the socket and passes them on to
        //  the peer engine.
        i_thread *source_thread = handler_threads [current_handler_thread];

        //  Create the pipe to the newly created engine.
        i_mux *mux = peer_engine->get_mux ();
        pipe_t *pipe = new pipe_t (source_thread, demux,
            peer_thread, mux, mux->get_swap_size ());
        zmq_assert (pipe);

        //  Bind new engine to the source end of the pipe.
        command_t demux_cmd;
        demux_cmd.init_attach_pipe_to_demux (demux, pipe);
        thread->send_command (source_thread, demux_cmd);

        //  Bind the peer to the destination end of the pipe.
        command_t mux_cmd;
        mux_cmd.init_attach_pipe_to_mux (mux, pipe);
        thread->send_command (peer_thread, mux_cmd);
    }
    else {
        //  Create mux for the sender_engine
        mux_t *mux = new mux_t (bp_hwm, bp_lwm);

        //  Create the engine to take care of the connection.
        //  TODO: make buffer size configurable by user
        bp_tcp_sender_t *engine = new bp_tcp_sender_t (mux, fd, peer_name);
        zmq_assert (engine);
        engine->start (thread, handler_threads [current_handler_thread]);

        //  The newly created engine serves as a local destination of messages
        //  I.e. it sends messages received from the peer engine to the socket.
        i_thread *destination_thread =
            handler_threads [current_handler_thread];

        //  Create the pipe to the newly created engine.
        i_demux *demux = peer_engine->get_demux ();
        pipe_t *pipe = new pipe_t (peer_thread, demux,
            destination_thread, mux, mux->get_swap_size ());
        zmq_assert (pipe);

        //  Bind new engine to the destination end of the pipe.
        command_t mux_cmd;
        mux_cmd.init_attach_pipe_to_mux (mux, pipe);
        thread->send_command (destination_thread, mux_cmd);

        //  Bind the peer to the source end of the pipe.
        command_t demux_cmd;
        demux_cmd.init_attach_pipe_to_demux (demux, pipe);
        thread->send_command (peer_thread, demux_cmd);
    }

    //  Move to the next thread to get round-robin balancing of engines.
    current_handler_thread ++;
    if (current_handler_thread == handler_threads.size ())
        current_handler_thread = 0;
}

void zmq::bp_tcp_listener_t::out_event ()
{
    //  We will never get POLLOUT when listening for incoming connections.
    zmq_assert (false);
}

void zmq::bp_tcp_listener_t::timer_event ()
{
    //  This class doesn't use timers.
    zmq_assert (false);
}

void zmq::bp_tcp_listener_t::unregister_event ()
{
    //  TODO: Implement full-blown shut-down mechanism.
    //  For now, we'll just close the underlying socket.
    poller->rm_fd (handle);
    listener.close ();
}

const char *zmq::bp_tcp_listener_t::get_arguments ()
{
    zmq_snprintf (arguments, sizeof (arguments), "zmq.tcp://%s",
        listener.get_interface ());
    return arguments;
}

void zmq::bp_tcp_listener_t::revive ()
{
    zmq_assert (false);
}

void zmq::bp_tcp_listener_t::head ()
{
    zmq_assert (false);
}

void zmq::bp_tcp_listener_t::receive_from ()
{
    zmq_assert (false);
}

void zmq::bp_tcp_listener_t::send_to ()
{
    zmq_assert (false);
}
