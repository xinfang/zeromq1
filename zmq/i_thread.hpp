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

#ifndef __ZMQ_I_THREAD_HPP_INCLUDED__
#define __ZMQ_I_THREAD_HPP_INCLUDED__

#include "command.hpp"

namespace zmq
{

    //  This interface should be exposed by all thread implementations.
    //  It allows engines to communicate with different threads.

    struct i_thread
    {
        //  The destructor shouldn't be virtual, however, not defining it as
        //  such results in compiler warnings with some compilers.
        virtual ~i_thread () {};

        //  Returns ID of the thread - the ID can be passed to different
        //  threads so that they can send commands back to the current thread
        virtual int get_thread_id () = 0;

        //  Sends command to a different thread
        virtual void send_command (int destination_thread_id_,
            const command_t &command_) = 0;
    };

}

#endif
