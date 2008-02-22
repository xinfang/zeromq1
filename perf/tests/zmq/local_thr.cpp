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

#include "../../transports/zmq.hpp"
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"
#include "../../workers/raw_receiver.hpp"

#include "./test.hpp"

void *worker_function (void *);

int main (int argc, char *argv [])
{
    if (argc != 1){
        printf ("Usage: local\n");
        return 1;
    }

    //  Main results results
    std::string filename ("in.dat");
    pthread_t workers [TEST_THREADS];
    worker_args_t *w_args;

    FILE *output = ::fopen (filename.c_str (), "w");
    assert (output);

    int msg_count;
    size_t msg_size;

    char file_name [255];
    memset (file_name, '\0', sizeof (file_name));

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;

    perf::time_instant_t min_start_time;
    perf::time_instant_t max_stop_time;

    // throughput [msgs/s]
    unsigned long long msg_thput;
    // throughput [Mb/s]
    unsigned long long tcp_thput;

    for (int i = 0; i < TEST_MSG_SIZE_STEPS; i++) {
   
        min_start_time = std::numeric_limits<unsigned long long>::max ();
        max_stop_time = 0;

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
        printf ("Threads: %i\n", TEST_THREADS);
        printf ("Message size: %i\n", msg_size);
        printf ("Number of messages in the throughput test: %i (per thread)\n", msg_count);

        {
            perf::zmq_t transport (true, "0.0.0.0", PORT_NUMBER, TEST_THREADS);

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

                // read results from finished worker file
                snprintf (file_name, sizeof (file_name) - 1, "%i_%i_in.dat", 
                    msg_size, j);

                perf::read_times_1f (&start_time, &stop_time, file_name);

                if (start_time < min_start_time)
                    min_start_time = start_time;

                if (stop_time > max_stop_time)
                    max_stop_time = stop_time;

                // delete file
                rc = remove (file_name);
                assert (rc == 0);
            }

        }

        printf ("Test end\n");
        fflush (stdout);

        printf ("Test time: %llu [ms]\n", (max_stop_time - min_start_time) / 
            (long long)1000);

        // throughput [msgs/s]
        msg_thput = ((long long) 1000000 * (long long) msg_count * (long long)TEST_THREADS 
            ) / (max_stop_time - min_start_time);

        // throughput [Mb/s]
        tcp_thput = (msg_thput * msg_size * 8) /(long long) 1000000;
                
        printf ("Your average throughput is %llu msgs/s\n", msg_thput);
        printf ("Your average throughput is %llu Mb/s\n\n", tcp_thput);

        fprintf (output, "%i %i %llu %llu\n", msg_size, msg_count * TEST_THREADS,
            min_start_time, max_stop_time);
    }
    
    fclose (output);

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

    perf::raw_receiver_t worker (w_args->msg_count);
    worker.run (*w_args->transport, prefix, w_args->id);

    return NULL;
}
