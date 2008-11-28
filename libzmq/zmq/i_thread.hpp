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

#ifndef __ZMQ_I_THREAD_HPP_INCLUDED__
#define __ZMQ_I_THREAD_HPP_INCLUDED__

#include <zmq/export.hpp>
#include <zmq/command.hpp>

namespace zmq
{

    //  This interface can be used for inter-thread communication. Thread
    //  context is uniquely specified by i_thread pointer. Sending a message
    //  between threads boils down to sending it between the thread contexts.

    struct i_thread
    {
        //  Wait for thread termination.
        ZMQ_EXPORT virtual ~i_thread () {};

        //  Returns unique ID of the thread.
        virtual int get_thread_id () = 0;

        //  Sends command to a different thread.
        virtual void send_command (i_thread *destination_,
            const struct command_t &command_) = 0;

		//  Initiate thread termination.
		virtual void stop () = 0;
    };

}

#endif
