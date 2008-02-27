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

//  Test whether API engine is able to send a single message to itself

#include <assert.h>

#include "../zmq/dispatcher.hpp"
#include "../zmq/api_engine.hpp"
using namespace zmq;

int main ()
{
    //  Create a dispatcher with a single engine plugged-in
    dispatcher_t dispatcher (1);
    api_engine_t engine (&dispatcher, 0);

    for (int msg_nbr = 0; msg_nbr != 10000; msg_nbr++) {

        //  Send a message to yourself
        cmsg_t msg_out = {(void*) "ABCDEF", 6, NULL};
        engine.send (0, msg_out);

        //  Receive the message
        cmsg_t msg_in;
        init_cmsg (msg_in);
        engine.receive (&msg_in);
        assert (msg_in.size == 6);
        free_cmsg (msg_in);
    }

    return 0;
}
