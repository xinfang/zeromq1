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

#include "../../transports/zmq.hpp"
#include "../scenarios/thr.hpp"

void *worker_function (void*);

int main (int argc, char *argv [])
{

    if (argc != 6) { 
        printf ("Usage: remote_thr <global_locator IP> <global_locator port> "
            "<message size> <message count> <number of threads>\n"); 
        return 1;
    }

    int thread_count = atoi (argv [5]);

    pthread_t *workers = new pthread_t [thread_count];
    perf::thr_worker_args_t *w_args =
        new perf::thr_worker_args_t [thread_count];

    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        w_args [thread_nbr].id = thread_nbr;
        w_args [thread_nbr].msg_size = atoi (argv [3]);
        w_args [thread_nbr].roundtrip_count = atoi (argv [4]);
        w_args [thread_nbr].listen_ip = NULL;
        w_args [thread_nbr].listen_port_base = 0;
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
    
    delete [] w_args;
    delete [] workers;

    return 0;
}

void *worker_function (void *args_)
{
    perf::thr_worker_args_t *w_args = (perf::thr_worker_args_t*)args_;

    char queue_name [32];
    char exchange_name [32];

    snprintf (queue_name, sizeof (queue_name), "Q%i",w_args->id);
    snprintf (exchange_name, sizeof (exchange_name), "E%i", w_args->id);

    perf::zmq_t transport (true, queue_name, exchange_name, w_args->locator_ip,
        w_args->locator_port, NULL, 0);

    perf::remote_thr (&transport, w_args->msg_size, w_args->roundtrip_count);

    return NULL;
}
