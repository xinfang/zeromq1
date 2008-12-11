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

#include <zmq/demux.hpp>
#include <zmq/raw_message.hpp>

zmq::demux_t::demux_t ()
{
}

zmq::demux_t::~demux_t ()
{
}

void zmq::demux_t::send_to (pipe_t *pipe_)
{
    //  Associate demux with a new pipe.
    pipes.push_back (pipe_);
}

bool zmq::demux_t::write (message_t &msg_)
{
    //  Underlying layers work with raw_message_t, layers above use message_t.
    //  Demux is the component that translates between the two.
    raw_message_t *msg = (raw_message_t*) &msg_;

    //  For delimiters, the algorithm is straighforward.
    //  Send it to all the pipes. Don't care about pipe limits.
    //  TODO: Delimiters are special, they should be passed via different
    //  codepath.
    if (msg->content == (message_content_t*) raw_message_t::delimiter_tag) {
        for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
            (*it)->write (msg);
        raw_message_init (msg, 0);
        return true;
    }

    int pipes_count = pipes.size ();

    //  If there are no pipes available, simply drop the message.
    if (pipes_count == 0) {
        raw_message_destroy (msg);
        raw_message_init (msg, 0);
        return true;
    }

    //  First check whether all the pipes are available for writing.
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        if (!(*it)->check_write ())
            return false;

    //  For VSMs the copying is straighforward.
    if (msg->content == (message_content_t*) raw_message_t::vsm_tag) {
        for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
            (*it)->write (msg);
        raw_message_init (msg, 0);
        return true;
    }

    //  Optimisation for the case where's there only a single pipe
    //  to send the message to - no refcount adjustment (i.e. atomic
    //  operations) needed.
    if (pipes_count == 1) {
        (*pipes.begin ())->write (msg);
        raw_message_init (msg, 0);
        return true;
    }

    //  There are at least 2 destinations for the message. That means we have
    //  to deal with reference counting. First add N-1 references to
    //  the content (we are holding one reference anyway, that's why -1).
    if (msg->shared)
        msg->content->refcount.add (pipes_count - 1);
    else {
        msg->content->refcount.set (pipes_count);
        msg->shared = true;
    }

    //  Push the message to all the destinations.
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        (*it)->write (msg);

    //  Detach the original message from the data buffer.
    raw_message_init (msg, 0);

    return true;
}

void zmq::demux_t::flush ()
{
    //  Flush all the present messages to the pipes.
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        (*it)->flush ();
}

bool zmq::demux_t::empty ()
{
    return pipes.empty ();
}

void zmq::demux_t::release_pipe (pipe_t *pipe_)
{
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++) {
        if (*it == pipe_) {
            pipes.erase (it);
            return;
        }
    }
}

void zmq::demux_t::initialise_shutdown ()
{
    //  Broadcast 'terminate_writer' to all associated pipes.
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        (*it)->terminate_writer ();
}
