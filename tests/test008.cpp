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

//  Test simple API->backend protocol over TCP socket->API message transfer
//  (ping-pong style)

#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "../zmq/dispatcher.hpp"
#include "../zmq/api_engine.hpp"
#include "../zmq/bp_engine.hpp"
#include "../zmq/poll_thread.hpp"
using namespace zmq;

void *ping_pong_routine (void*)
{
    dispatcher_t dispatcher (2);
    api_engine_t api (&dispatcher, 0);
    bp_engine_t bp (&dispatcher, 1, false, "127.0.0.1", 5555, 0, 0, 8192, 8192);
    poll_thread_t poll_thread (&bp);

    //  Do ping-pong in a loop
    for (int msg_nbr = 0; msg_nbr != 10000; msg_nbr ++) {
        cmsg_t msg_out = {(void*) "ABCDEF", 6, NULL};
        api.send (1, msg_out);
        cmsg_t msg_in;
        init_cmsg (msg_in);
        api.receive (&msg_in);
        assert (msg_in.size == 6);
        free_cmsg (msg_in);   
    }

    return NULL;
}

void *echo_routine (void*)
{
    dispatcher_t dispatcher (2);
    api_engine_t api (&dispatcher, 0);
    bp_engine_t bp (&dispatcher, 1, true, "0.0.0.0", 5555, 0, 0, 8192, 8192);
    poll_thread_t poll_thread (&bp);

    //  Echo messages in a loop
    for (int msg_nbr = 0; msg_nbr != 10000; msg_nbr ++) {
        cmsg_t msg;
        init_cmsg (msg);
        api.receive (&msg);
        assert (msg.size == 6);
        api.send (1, msg);
    }

    return NULL;
}

int main ()
{

    //  Start the echo thread
    pthread_t echo;
    int rc = pthread_create (&echo, NULL, echo_routine, NULL);
    assert (rc == 0);

    sleep (1);

    //  Start the ping-pong thread
    pthread_t ping_pong;
    rc = pthread_create (&ping_pong, NULL, ping_pong_routine, NULL);
    assert (rc == 0);

    //  Wait till worker threads terminate
    rc = pthread_join (echo, NULL);
    assert (rc == 0);
    rc = pthread_join (ping_pong, NULL);
    assert (rc == 0);

    return 0;
}
