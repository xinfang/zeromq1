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

#include <zmq/mux.hpp>
#include <zmq/raw_message.hpp>

zmq::mux_t::mux_t () :
    current (0)
{
}

zmq::mux_t::~mux_t ()
{
}

void zmq::mux_t::receive_from (pipe_t *pipe_, bool shutting_down_)
{
    //  Associate new pipe with the mux object.
    pipes.push_back (pipe_);

    //  If we are already in shut down phase, initiate shut down of the pipe
    //  immediately.
    if (shutting_down_)
        pipe_->terminate_reader ();
}

bool zmq::mux_t::read (message_t *msg_)
{
    //  Underlying layers work with raw_message_t, layers above use message_t.
    //  Mux is the component that translates between the two.
    raw_message_t *msg = (raw_message_t*) msg_;

    //  Deallocate old content of the message.
    raw_message_destroy (msg);

    //  Round-robin over the pipes to get next message.
    for (int to_process = pipes.size (); to_process != 0; to_process --) {

        bool retrieved = pipes [current]->read ((raw_message_t*) msg_);

        current ++;
        if (current == pipes.size ())
            current = 0;

        if (retrieved)
            return true;
    }

    //  No message is available. Initialise the output parameter
    //  to be a 0-byte message.
    raw_message_init (msg, 0);
    return false;
}

void zmq::mux_t::release_pipe (pipe_t *pipe_)
{
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        if (*it == pipe_) {

            //  At this point pipe is physically destroyed.
            delete *it;

            //  Remove the pipe from the list.
            pipes.erase (it);
            if (current == pipes.size ())
                current = 0;
            return;
        }

    //  There's a bug in shut down mechanism!
    assert (false);
}

void zmq::mux_t::initialise_shutdown ()
{
    //  Broadcast 'terminate_reader' to all the pipes associated with the mux.
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        (*it)->terminate_reader ();
}
