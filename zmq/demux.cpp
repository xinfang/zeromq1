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

#include "demux.hpp"
#include "raw_message.hpp"

zmq::demux_t::demux_t (bool load_balance_) :
    load_balance (load_balance_),
    current (0)
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

    //  If there is no destination to send the message return straight away.
    int pipes_count = pipes.size ();
    if (pipes_count == 0)
        return false;

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

    //  Load balancing is easy. The message is sent to exactly one pipe
    //  thus there's no need for copying.
    if (load_balance) {
        int old_current = current;
        while (true) {

            //  Move current to the next pipe.
            current ++;
            if (current == pipes_count)
                current = 0;

            //  Try to write the message to the pipe.
            if (pipes [current]->check_write ()) {
                pipes [current]->write (msg);
                break;
            }

            //  If all the pipes are non-writeable fail.
            if (current == old_current)
                return false;
        }
        raw_message_init (msg, 0);
        return true;
    }

    //  This is data distribution path (as opposed to load balancing).

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

void zmq::demux_t::terminate_pipes () 
{
    //  Write delimiters to the pipes.
    for (pipes_t::iterator it = pipes.begin (); it != pipes.end (); it ++)
        (*it)->write_delimiter ();

    //  Remove all pointers to pipes.
    pipes.clear ();
}

void zmq::demux_t::destroy_pipe (pipe_t *pipe_)
{
    //  Find the pipe.
    pipes_t::iterator it = std::find (pipes.begin (), pipes.end (), pipe_);

    //  Send delimiter to the pipe and drop the pointer.
    if (it != pipes.end ()) {
        pipe_->write_delimiter ();
        pipes.erase (it);
    }
}
