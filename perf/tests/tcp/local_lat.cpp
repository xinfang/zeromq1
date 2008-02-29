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
#include <string>
#include <limits>

#include "../../transports/tcp.hpp"
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"
#include "../../workers/raw_ping_pong.hpp"

#include "./test.hpp"

using namespace std;

void *worker_function (void *);

int main (int argc, char *argv [])
{
    if (argc != 1){
        printf ("Usage: local\n");
        exit (0);
    }

    if (TEST_THREADS != 1) {
        printf ("Latency test with more than 1 thread does not nake sense.\n");
        assert (0);
    }

    pthread_t workers [TEST_THREADS];
    worker_args_t *w_args;

    int msg_size;
    int msg_count;

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;
    
    //  Main results results
    string filename ("timing.dat");

    FILE *output = ::fopen (filename.c_str (), "w");
    assert (output);

    for (int i = 0; i < TEST_MSG_SIZE_STEPS; i++) {

        msg_size = TEST_MSG_SIZE_START * (0x1 << i);

        if (msg_size < SYS_BREAK) {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE * msg_size + SYS_OFF));
            msg_count /= TEST_THREADS;
            msg_count /= SYS_LAT_DEN;
        } else {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE_BIG * msg_size + SYS_OFF_BIG));
            msg_count /= TEST_THREADS;
            msg_count /= SYS_LAT_DEN;
        }

        printf ("Threads: %i\n", TEST_THREADS);
        printf ("Message size: %i\n", msg_size);
        printf ("Number of messages in the latency test: %i\n", msg_count);

        for (int j = 0; j < TEST_THREADS; j++) {
            w_args = new worker_args_t;
            w_args->id = j;
            w_args->msg_size = msg_size;
            w_args->msg_count = msg_count;

            int rc = pthread_create (&workers [j], NULL, worker_function,
                (void*)w_args);
            assert (rc == 0);
        }

        for (int j = 0; j < TEST_THREADS; j++) {
            int rc = pthread_join (workers [j], NULL);
            assert (rc == 0);

            char file_prefix [255];
            memset (file_prefix, '\0', sizeof (file_prefix));
            snprintf (file_prefix, sizeof (file_prefix) - 1, "%i_%i_", msg_size, j);
            perf::read_times_2f (&start_time, &stop_time, file_prefix);

            // write results to the main file
            fprintf (output, "%i %i %llu %llu\n", msg_size, msg_count, start_time, stop_time);

            // delete files
            snprintf (file_prefix, sizeof (file_prefix) - 1, "%i_%i_in.dat", 
                msg_size, j);
            rc = remove (file_prefix);
            assert (rc == 0);

            snprintf (file_prefix, sizeof (file_prefix) - 1, "%i_%i_out.dat", 
                msg_size, j);
            rc = remove (file_prefix);
            assert (rc == 0);
        }
    }

    fclose (output);

    return 0;
}

void *worker_function (void *args_)
{
    // args struct
    worker_args_t *w_args = (worker_args_t*)args_;
    
    // file prefix
    char prefix [20];
    memset (prefix, '\0', sizeof (prefix));
    snprintf (prefix, sizeof (prefix) - 1, "%i_%i_", w_args->msg_size, 
        w_args->id);

    perf::tcp_t transport (true, "0.0.0.0", PORT_NUMBER + w_args->id, false);
    perf::raw_ping_pong_t worker (w_args->msg_count, w_args->msg_size);
    worker.run (transport, prefix);

    delete w_args;
}

