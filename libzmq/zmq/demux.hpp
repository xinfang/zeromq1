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

#ifndef __ZMQ_DEMUX_HPP_INCLUDED__
#define __ZMQ_DEMUX_HPP_INCLUDED__

#include <assert.h>
#include <vector>
#include <algorithm>

#include <zmq/message.hpp>
#include <zmq/pipe.hpp>

namespace zmq
{

    //  Object to distribute messages to outbound pipes.

    class demux_t
    {
    public:

        demux_t ();
        ~demux_t ();

        //  Start sending messages to the specified pipe.
        void send_to (pipe_t *pipe_);

        //  Send the message (actual send is delayed till next flush). Function
        //  returns true if message is written to at least one pipe. The message
        //  is cleared in that case. If it returns false, message wasn't written
        //  to a pipe and it is left intact.
        bool write (message_t &msg_);

        //  Flush the messages.
        void flush ();

        //  Returns true if there are no pipes attached.
        bool empty ();

        //  Drop references to the specified pipe.
        void release_pipe (pipe_t *pipe_);

        //  Initiate shutdown of all associated pipes.
        void initialise_shutdown ();

        //  Returns true if there are no pipes to send messages to.
        inline bool no_pipes ()
        {
            return pipes.empty ();
        }

        //  Returns true if demux state is dead or new.
        bool dead ();

    private:

        enum demux_state_t {

            //  Newly created demux, no pipe attached.
            demux_new,

            //  Demux has at least one pipe attached.
            //  Engine processed recv_from command.
            demux_connected,
            
            //  All pipes deleted.
            demux_dead
        };

        //  The list of outbound pipes.
        typedef std::vector <pipe_t*> pipes_t;
        pipes_t pipes;

        //  TODO: disable copying

        //  Demux state.
        demux_state_t state;
    };

}

#endif
