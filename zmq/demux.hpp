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

#ifndef __ZMQ_DEMUX_HPP_INCLUDED__
#define __ZMQ_DEMUX_HPP_INCLUDED__

#include <assert.h>
#include <vector>
#include <algorithm>

#include "message.hpp"
#include "pipe.hpp"

namespace zmq
{

    //  Object to distribute messages to outbound pipes.

    class demux_t
    {
    public:

        demux_t (bool load_balance_);
        ~demux_t ();

        //  Start sending messages to the specified pipe.
        void send_to (pipe_t *pipe_);

        //  Send the message (actual send is delayed till next flush). Function
        //  return true if message is written to at least one pipe. The message
        //  is cleared in that case. If it returns false, message wasn't written
        //  to a pipe and it is left intact.
        int write (message_t &msg_);

        //  Flush the messages.
        void flush ();

        //  Write a delimiter to each pipe.
        void terminate_pipes ();

        //  Write a delimiter to the specified pipe.
        void destroy_pipe (pipe_t *pipe_);

    private:

        //  If true, messages are load-balanced among pipes. If false, each
        //  message is sent to all the pipes.
        bool load_balance;

        //  Index of the current pipe w.r.t. load balancing. If the object
        //  is in data distribution mode this variable is unused.
        int current;

        //  The list of outbound pipes.
        typedef std::vector <pipe_t*> pipes_t;
        pipes_t pipes;

        //  TODO: disable copying
    };

}

#endif
