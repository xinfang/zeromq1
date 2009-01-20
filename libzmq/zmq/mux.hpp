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

#ifndef __ZMQ_MUX_HPP_INCLUDED__
#define __ZMQ_MUX_HPP_INCLUDED__

#include <assert.h>
#include <vector>

#include <zmq/export.hpp>
#include <zmq/message.hpp>
#include <zmq/pipe.hpp>

namespace zmq
{

    //  Object to aggregate messages from inbound pipes.

    class mux_t
    {
    public:

        ZMQ_EXPORT mux_t ();
        ZMQ_EXPORT ~mux_t ();

        //  Adds a pipe to receive messages from.
        ZMQ_EXPORT void receive_from (pipe_t *pipe_);

        //  Returns a message, if available. If not, returns false.
        ZMQ_EXPORT bool read (message_t *msg_);

        //  Returns true if there are no pipes attached.
        ZMQ_EXPORT bool empty ();

        //  Drop references to the specified pipe.
        ZMQ_EXPORT void release_pipe (pipe_t *pipe_);

        //  Initiate shutdown of all associated pipes.
        ZMQ_EXPORT void initialise_shutdown ();

    private:

        //  The list of inbound pipes.
        typedef std::vector <pipe_t*> pipes_t;
        pipes_t pipes;

        //  Pipe to retrieve next message from. The messages are retrieved
        //  from the pipes in round-robin fashion (a.k.a. fair queueing).
        pipes_t::size_type current;

        //  TODO: disable copying
    };

}

#endif
