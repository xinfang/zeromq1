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

//  Test whether messages from two API engines can be received by a single
//  API engine

#include <assert.h>
#include <pthread.h>

#include "../zmq/dispatcher.hpp"
#include "../zmq/api_engine.hpp"
using namespace zmq;

void *worker_routine (void *arg)
{
    api_engine_t *engine = (api_engine_t*) arg;

    //  Send messages
    for (int msg_nbr = 0; msg_nbr != 10000; msg_nbr ++) {
        cmsg_t msg = {(void*) "ABCDEF", 6, NULL};
        engine->send (0, msg);
    }

    return NULL;
}

int main ()
{
    //  Create a dispatcher with three API engines plugged-in
    dispatcher_t dispatcher (3);
    api_engine_t engine0 (&dispatcher);
    api_engine_t engine1 (&dispatcher);
    api_engine_t engine2 (&dispatcher);

    //  Start the sender threads
    pthread_t worker1;
    int rc = pthread_create (&worker1, NULL, worker_routine, (void*) &engine1);
    assert (rc == 0);
    pthread_t worker2;
    rc = pthread_create (&worker2, NULL, worker_routine, (void*) &engine2);
    assert (rc == 0);

    //  Receive messages
    for (int msg_nbr = 0; msg_nbr != 20000; msg_nbr++) {
        cmsg_t msg;
        init_cmsg (msg);
        engine0.receive (&msg);
        assert (msg.size == 6);
        free_cmsg (msg);
    }

    //  Wait till worker threads terminate
    rc = pthread_join (worker2, NULL);
    assert (rc == 0);
    rc = pthread_join (worker1, NULL);
    assert (rc == 0);

    return 0;
}
