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
#include <vector>
#include <limits>

#include "../../transports/zmq.hpp"
#include "../../helpers/time.hpp"
#include "../scenarios/thr.hpp"

void *worker_function (void*);

int main (int argc, char *argv [])
{
    if (argc != 8){
        printf ("Usage: local_thr <global_locator IP> <global_locator port> "
            "<listen IP> <listen port> <message size> <message count> "
            "<number of threads>\n");
        return 1;
    }

//    printf ("estimating CPU frequency...\n");
//    uint64_t frq = perf::estimate_cpu_frequency ();
//    printf ("your CPU frequncy is %.2f GHz\n", ((double) frq) / 1000000000);

    int thread_count = atoi (argv [7]);
    
    pthread_t *workers = new pthread_t [thread_count];
    perf::thr_worker_args_t *w_args =
        new perf::thr_worker_args_t [thread_count];

    printf ("threads: %i\n", thread_count);
    printf ("message size: %i\n", atoi (argv [5]));
    printf ("roundtrip count: %i\n", atoi (argv [6]));

    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        w_args [thread_nbr].id = thread_nbr;
        w_args [thread_nbr].msg_size = atoi (argv [5]);
        w_args [thread_nbr].roundtrip_count = atoi (argv [6]);
        w_args [thread_nbr].listen_ip = argv [3];
        w_args [thread_nbr].listen_port_base = atoi (argv [4]);
        w_args [thread_nbr].locator_ip = argv [1];
        w_args [thread_nbr].locator_port = atoi (argv [2]);

        w_args [thread_nbr].start_time = 0;
        w_args [thread_nbr].stop_time = 0;

        int rc = pthread_create (&(workers [thread_nbr]), NULL, worker_function, 
            (void*)(w_args + thread_nbr));
        assert (rc == 0);
    }

    
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        int rc = pthread_join (workers [thread_nbr], NULL); 
        assert (rc == 0);
    }

    // gather results from thr_worker_args_t structures
    perf::time_instant_t min_start_time  = 
        std::numeric_limits<uint64_t>::max ();
    perf::time_instant_t max_stop_time = 0;

    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        if (w_args [thread_nbr].start_time < min_start_time)
            min_start_time = w_args [thread_nbr].start_time;

        if (w_args [thread_nbr].stop_time > max_stop_time)
            max_stop_time = w_args [thread_nbr].stop_time;

    }

    double test_time = (double)(max_stop_time - min_start_time) /
        (double) 1000000;

    printf ("test time: %.2f [ms]\n", test_time);

    // throughput [msgs/s]
    unsigned long msg_thput = ((long) 1000000000 * 
        (unsigned long) atoi (argv [6]) * (unsigned long)thread_count ) / 
        (unsigned long)(max_stop_time - min_start_time);

    unsigned long tcp_thput = (msg_thput * atoi (argv [5]) * 8) /
        (unsigned long) 1000000;

    printf ("Your average throughput is %lu [msgs/s]\n", msg_thput);
    printf ("Your average throughput is %lu [Mb/s]\n\n", tcp_thput);

    FILE *output = ::fopen ("tests.dat", "a");
    assert (output);
    fprintf (output, "%i,%i,%i,%.2f,%lu,%lu\n", thread_count, atoi (argv [6]), 
        atoi (argv [5]), test_time, msg_thput, tcp_thput);
    fclose (output);

    delete [] workers;
    delete [] w_args;

    return 0;
}

void *worker_function (void *args_)
{
    // args struct
    perf::thr_worker_args_t *w_args = (perf::thr_worker_args_t*)args_;
   
    char queue_name [32];
    char exchange_name [32];

    snprintf (queue_name, sizeof (queue_name), "Q%i",w_args->id);
    snprintf (exchange_name, sizeof (exchange_name), "E%i", w_args->id);

    perf::zmq_t transport (false, queue_name, exchange_name, w_args->locator_ip,
        w_args->locator_port, w_args->listen_ip, 
        w_args->listen_port_base + 2 * w_args->id);

    std::vector<perf::time_instant_t> start_stop = perf::local_thr (&transport, 
        w_args->msg_size, w_args->roundtrip_count);

    // store results 
    w_args->start_time = start_stop [0];
    w_args->stop_time = start_stop [1];
    
    return NULL;
}
