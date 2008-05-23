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

void *zmq::mux_t::read ()
{
    for (int to_process = pipes.size (); to_process != 0; to_process --) {
        void* msg = pipes [current]->read ();
        if (pipes [current]->eop ()) {
            delete pipes [current];
            pipes.erase (pipes.begin () + current);
        } else {
            current ++;
        }
        if (current == pipes.size ())
            current = 0;
        if (msg)
            return msg;
    }
    return NULL;
}

void zmq::mux_t::terminate_pipes()
{
    for (int i = 0; i < pipes.size () ; ++i)
        pipes [i]->send_destroy_pipe ();
}
