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
#include "../../helpers/time.hpp"
#include "../../workers/raw_sender.hpp"

#include "./test.hpp"

void *worker_function (void *);

int main (int argc, char *argv [])
{

    if (argc != 2) {
        printf ("Usage: remote <ip address where \'local\' runs>\n");
        return 1;
    }

    pthread_t workers [TEST_THREADS];
    worker_args_t *w_args;

    size_t msg_size;
    int msg_count;

    for (int i = 0; i < TEST_MSG_SIZE_STEPS; i++) {

        msg_size = TEST_MSG_SIZE_START * (0x1 << i);

        if (msg_size < SYS_BREAK) {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE * msg_size + SYS_OFF));
        } else {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE_BIG * msg_size + SYS_OFF_BIG));
        }

        msg_count /= TEST_THREADS;

//        msg_count = TEST_MSG_COUNT_THRPUT;

        {
            perf::zmq_t transport (false, argv [1], PORT_NUMBER, TEST_THREADS);
            
            for (int j = 0; j < TEST_THREADS; j++) {
                w_args = new worker_args_t;
                w_args->id = j;
                w_args->msg_size = msg_size;
                w_args->msg_count = msg_count;
                w_args->transport = &transport;

                int rc = pthread_create (&workers [j], NULL, worker_function,
                    (void*)w_args);
                assert (rc == 0);
            }
            
            for (int j = 0; j < TEST_THREADS; j++) {
                int rc = pthread_join (workers [j], NULL);
                assert (rc == 0);
            }
        }

        sleep (2); // Wait till new listeners are started by the 'local'

    }

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

    perf::raw_sender_t worker (w_args->msg_count, w_args->msg_size);
    worker.run (*w_args->transport, prefix, w_args->id);

    return NULL;
}
