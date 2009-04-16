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

#include <vector>
#include <algorithm>

#include <zmq/message.hpp>
#include <zmq/pipe.hpp>

namespace zmq
{

    //  Object to aggregate messages from inbound pipes.

    class mux_t
    {
    public:

        mux_t ();
        ~mux_t ();

        //  Adds a pipe to receive messages from.
        void receive_from (pipe_t *pipe_);

        //  Revives a stalled pipe.
        void revive (pipe_t *pipe_);

        //  Subscribe to particular messages.
        void subscribe (const char *criteria_);

        //  Returns a message, if available. If not, returns false.
        bool read (message_t *msg_);

        //  Returns true if there are no pipes attached.
        bool empty ();

        //  Drop references to the specified pipe.
        void release_pipe (pipe_t *pipe_);

        //  Initiate shutdown of all associated pipes.
        void initialise_shutdown ();

    private:

        //  The list of inbound pipes. The active pipes are occupying indices
        //  from 0 to active-1. Suspended pipes occupy indices from 'active'
        //  to the end of the array.
        typedef std::vector <pipe_t*> pipes_t;
        pipes_t pipes;

        //  The number of active pipes.
        pipes_t::size_type active;

        //  Pipe to retrieve next message from. The messages are retrieved
        //  from the pipes in round-robin fashion (a.k.a. fair queueing).
        pipes_t::size_type current;

        //  Swaps pipes at specified indices. 
        inline void swap_pipes (pipes_t::size_type i1_, pipes_t::size_type i2_)
        {
            std::swap (pipes [i1_], pipes [i2_]);
            pipes [i1_]->set_index (i1_);
            pipes [i2_]->set_index (i2_);
        }

        mux_t (const mux_t&);
        void operator = (const mux_t&);
    };

}

#endif
