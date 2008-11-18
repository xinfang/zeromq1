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

#ifndef __ZMQ_DEMUX_HPP_INCLUDED__
#define __ZMQ_DEMUX_HPP_INCLUDED__

#include <assert.h>
#include <vector>
#include <algorithm>

#include "export.hpp"
#include "message.hpp"
#include "pipe.hpp"

namespace zmq
{

    //  Object to distribute messages to outbound pipes.

    class demux_t
    {
    public:

        ZMQ_EXPORT demux_t ();
        ZMQ_EXPORT ~demux_t ();

        //  Start sending messages to the specified pipe.
        ZMQ_EXPORT void send_to (pipe_t *pipe_);

        //  Send the message (actual send is delayed till next flush).
        ZMQ_EXPORT void write (message_t &msg_);

        //  Flush the messages.
        ZMQ_EXPORT void flush ();

        //  Returns true if there are no pipes attached.
        ZMQ_EXPORT bool empty ();

        //  Drop references to the specified pipe.
        ZMQ_EXPORT void release_pipe (pipe_t *pipe_);

        //  Initiate shutdown of all associated pipes.
        ZMQ_EXPORT void initialise_shutdown ();

    private:

        //  The list of outbound pipes.
        typedef std::vector <pipe_t*> pipes_t;
        pipes_t pipes;

        //  TODO: disable copying
    };

}

#endif
