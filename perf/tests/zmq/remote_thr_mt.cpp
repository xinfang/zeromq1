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

#include <cstdlib>
#include <cstdio>

#include "../../transports/zmq.hpp"
#include "../../workers/raw_sender.hpp"

#include "./test.hpp"

void *worker_function (void *);

int main (int argc, char *argv [])
{

    if (argc != 4) {
        printf ("Usage: remote <\'global_locator\' IP> <\'global locator\' port> <message size>\n");
        return 1;
    }

    size_t msg_size;
    int msg_count;

    pthread_t workers [TEST_THREADS];
    worker_args_t *w_args;

//        msg_size = TEST_MSG_SIZE_START * (0x1 << i);
    msg_size = atoi (argv [3]);

    if (msg_size < SYS_BREAK) {
        msg_count = (int)((TEST_TIME * 100000) / 
            (SYS_SLOPE * msg_size + SYS_OFF));
    } else {
        msg_count = (int)((TEST_TIME * 100000) / 
            (SYS_SLOPE_BIG * msg_size + SYS_OFF_BIG));
    }

    msg_count /= TEST_THREADS;

    char *queue_base_name = new char [3];
    sprintf (queue_base_name, "Q0");
    char *exchange_base_name = new char [3];
    sprintf (exchange_base_name, "E0");

    printf ("Message size: %i\n", (int)msg_size);
    printf ("Number of messages in the throughput test: %i (per thread)\n", msg_count);
    
    for (int j = 0; j < TEST_THREADS; j++) {
        w_args = new worker_args_t;
        w_args->id = j;
        w_args->msg_size = msg_size;
        w_args->msg_count = msg_count;

        w_args->listen_ip = NULL;
        w_args->listen_port = 0;
        w_args->locator_ip = argv [1];
        w_args->locator_port = atoi (argv [2]);

        char *queue_name = new char [strlen (queue_base_name) + 1];
        strcpy (queue_name, queue_base_name);
        queue_name [1] += j;
        w_args->queue_name = queue_name;

        char *exchange_name = new char [strlen (exchange_base_name) + 1];
        strcpy (exchange_name, exchange_base_name);
        exchange_name [1] += j;
        w_args->exchange_name = exchange_name;

        int rc = pthread_create (&workers [j], NULL, worker_function,
              (void*)w_args);
            assert (rc == 0);
    }
            
    for (int j = 0; j < TEST_THREADS; j++) {
        int rc = pthread_join (workers [j], NULL);
        assert (rc == 0);
    }

    sleep (2); // Wait till 'local' writes results

    return 0;
}

void *worker_function (void *args_)
{
    // args struct
    worker_args_t *w_args = (worker_args_t*)args_;

    char prefix [20];
    memset (prefix, '\0', sizeof (prefix));
    snprintf (prefix, sizeof (prefix) - 1, "%i_%i_", w_args->msg_size,
        w_args->id);

//    printf ("worker start %i, Q %s, E %s, port %i\n", w_args->id, 
//        w_args->queue_name, w_args->exchange_name, w_args->listen_port);

    perf::zmq_t transport (true, w_args->queue_name, w_args->exchange_name, 
        w_args->locator_ip, w_args->locator_port, w_args->listen_ip, w_args->listen_port);


    perf::raw_sender_t worker (w_args->msg_count, w_args->msg_size);
    worker.run (transport, prefix);

//    printf ("sent %i messages, waititng for sync message...", w_args->msg_count);
    transport.receive_sync_message ();
//    printf ("OK\n");

//    printf ("worker stop %i\n", w_args->id);

    delete [] w_args->queue_name;
    delete [] w_args->exchange_name;
    delete w_args;

    return NULL;
}
