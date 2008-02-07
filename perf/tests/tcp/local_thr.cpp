/*
    Copyright (c) 2007 FastMQ Inc.

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
#include "../../workers/raw_receiver.hpp"

#include "./test.hpp"

using namespace std;

void *worker_function (void *);
void read_times_1f (perf::time_instant_t *, perf::time_instant_t *, 
    const char *);

int main (int argc, char *argv [])
{
    if (argc != 1){
        printf ("Usage: local\n");
        exit (0);
    }

    pthread_t workers [TEST_THREADS];
    worker_args_t *w_args;

    int msg_size;
    int msg_count;

    perf::time_instant_t min_start_time;
    perf::time_instant_t max_stop_time;
    
    //  Main results results
    string filename ("in.dat");

    FILE *output = ::fopen (filename.c_str (), "w");
    assert (output);

    // throughput [msgs/s]
    unsigned long long msg_thput;
    // throughput [Mb/s]
    unsigned long long tcp_thput;

    for (int i = 0; i < TEST_MSG_SIZE_STEPS; i++) {

        min_start_time = numeric_limits<unsigned long long>::max();
        max_stop_time = 0;
//        printf ("%llu\n", max_stop_time);

        msg_size = TEST_MSG_SIZE_START * (0x1 << i);

        if (msg_size < SYS_BREAK) {
            msg_count = (int)((TEST_TIME * 100000) / (SYS_SLOPE * msg_size + SYS_OFF));
            msg_count /= TEST_THREADS;
        } else {
            msg_count = (int)((TEST_TIME * 100000) / (SYS_SLOPE_BIG * msg_size + SYS_OFF_BIG));
            msg_count /= TEST_THREADS;
        }

        printf ("Threads: %i\n", TEST_THREADS);
        printf ("Message size: %i\n", msg_size);
        printf ("Number of messages in the throughput test: %i\n", msg_count);

        for (int j = 0; j < TEST_THREADS; j++) {
            w_args = new worker_args_t;
            w_args->id = j;
            w_args->msg_size = msg_size;
            w_args->msg_count = msg_count;

            int rc = pthread_create (&workers [j], NULL, worker_function, (void*)w_args);
            assert (rc == 0);
        }

        for (int j = 0; j < TEST_THREADS; j++) {
            int rc = pthread_join (workers [j], NULL);
            assert (rc == 0);

            perf::time_instant_t start_time;
            perf::time_instant_t stop_time;
            char prefix [255];
            memset (prefix, '\0', sizeof (prefix));
            snprintf (prefix, sizeof (prefix) - 1, "%i_%i_", msg_size, j);
            read_times_1f (&start_time, &stop_time, prefix);

            if (start_time < min_start_time)
                min_start_time = start_time;

            if (stop_time > max_stop_time)
                max_stop_time = stop_time;

            // delete file
            snprintf (prefix, sizeof (prefix) - 1, "%i_%i_in.dat", msg_size, j);
            rc = remove (prefix);
            assert (rc == 0);
        }

        printf ("Test time: %llu [ms]\n", (max_stop_time - min_start_time) / (long long)1000);

        // throughput [msgs/s]
        msg_thput = ((long long) 1000000 * (long long) msg_count * (long long)TEST_THREADS) /
            (max_stop_time - min_start_time);

        // throughput [Mb/s]
        tcp_thput = (msg_thput * msg_size * 8) /(long long) 1000000;
                
        printf ("Your average throughput is %llu msgs/s\n", msg_thput);
        printf ("Your average throughput is %llu Mb/s\n\n", tcp_thput);

        fprintf (output, "%i %i %llu %llu\n", msg_size, msg_count * TEST_THREADS, min_start_time, max_stop_time);

    }
  
    fclose (output);

    return 0;
}

void read_times_1f (perf::time_instant_t *start_time_,
    perf::time_instant_t *stop_time_, const char *prefix_)
{
    //  Load the results
    char filename [256];
    snprintf (filename, 256, "%sin.dat", prefix_);
    FILE *input = ::fopen (filename, "r");
    assert (input);
    fscanf (input, "%llu", start_time_);
    fscanf (input, "%llu", stop_time_);
    fclose (input);
}

void *worker_function (void *args_)
{
    // args struct
    worker_args_t *w_args = (worker_args_t*)args_;
    
    // file prefix
    char prefix [20];
    memset (prefix, '\0', sizeof (prefix));
    snprintf (prefix, sizeof (prefix) - 1, "%i_%i_", w_args->msg_size, w_args->id);

    perf::tcp_t transport (true, "0.0.0.0", PORT_NUMBER + w_args->id, false);
    perf::raw_receiver_t worker (w_args->msg_count);
    worker.run (transport, prefix);

    delete w_args;
}

