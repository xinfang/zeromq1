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

#include <zmq/mux.hpp>
#include <zmq/raw_message.hpp>
#include <zmq/err.hpp>
#include <zmq/i_engine.hpp>

zmq::mux_t::mux_t (int64_t swap_size_) :
    engine (NULL),
    swap_size (swap_size_),
    active (0),
    current (0)
{
}

zmq::mux_t::~mux_t ()
{
}

int64_t zmq::mux_t::get_swap_size ()
{
    return swap_size;
}

void zmq::mux_t::register_engine (i_engine *engine_)
{
    zmq_assert (engine == NULL);
    engine = engine_;
}

void zmq::mux_t::receive_from (pipe_t *pipe_)
{
    //  Associate new pipe with the mux object.
    pipes.push_back (pipe_);
    ++active;
    if (pipes.size () > active)
        swap_pipes (pipes.size () - 1, active - 1);
}

void zmq::mux_t::revive (pipe_t *pipe_)
{
    //  Revive an idle pipe.
    swap_pipes (pipe_->index (), active);
    ++active;

    if (engine)
        engine->revive ();
}

bool zmq::mux_t::read (message_t *msg_)
{
    //  Underlying layers work with raw_message_t, layers above use message_t.
    //  Mux is the component that translates between the two.
    raw_message_t *msg = (raw_message_t*) msg_;

    //  Deallocate old content of the message.
    raw_message_destroy (msg);

    //  Round-robin over the active pipes to get next message.
    for (pipes_t::size_type i = active; i != 0; i--) {

        //  Update current.
        current = (current + 1) % active;
        
        //  Try to read from current.
        if (pipes [current]->read ((raw_message_t*) msg_))
            return true;
    
        //  There's no message in the current pipe. Move it to idle list.
        swap_pipes (current, active - 1);
        --active;
    }

    //  No message is available. Initialise the output parameter
    //  to be a 0-byte message.
    raw_message_init (msg, 0);
    return false;
}

bool zmq::mux_t::empty ()
{
    return pipes.empty ();
}

void zmq::mux_t::release_pipe (pipe_t *pipe_)
{
    pipes_t::size_type i = pipe_->index ();
 
    //  Remove pipe from the active list.
    if (i < active) {
        swap_pipes (i, active - 1);
        i = --active;
    }
            
    //  Move the pipe to the end of the idle list and remove it.
    swap_pipes (i, pipes.size () - 1);
    pipes.pop_back ();
        
    //  At this point pipe is physically destroyed.
    delete pipe_;
}

void zmq::mux_t::initialise_shutdown ()
{
    //  Broadcast 'terminate_reader' to all the pipes associated with the mux.
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        (*it)->terminate_reader ();
}
