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

#include "mux.hpp"

zmq::mux_t::mux_t () :
    current (0)
{
}

zmq::mux_t::~mux_t ()
{
}

void zmq::mux_t::receive_from (pipe_t *pipe_)
{
    pipes.push_back (pipe_);
}

bool zmq::mux_t::read (cmsg_t *cmsg_)
{
    //  Check at most as many pipes as there are available, i.e do one
    //  round-robin cycle.
    for (int to_process = pipes.size (); to_process != 0; to_process --) {

        //  Try to get a message from the current pipe
        bool retrieved = pipes [current]->read (cmsg_);

        //  If we've read delimiter from the pipe, we can delete it.
        if (!retrieved && pipes [current]->eop ()) {
            delete pipes [current];
            pipes.erase (pipes.begin () + current);
            current --;
        }

        //  Advance current pointer (next step of the round-robin).
        current ++;
        if (current == pipes.size ())
            current = 0;

        //  Return if the message was fetched already.
        if (retrieved)
            return true;
    }

    //  There is no message available at the moment.
    return false;
}

void zmq::mux_t::terminate_pipes()
{
    for (int i = 0; i < pipes.size () ; ++i)
        pipes [i]->send_destroy_pipe ();
}
