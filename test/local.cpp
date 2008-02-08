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

#define PREFIX "" 

#include "../perf/transports/tcp.hpp"
#include "../perf/transports/zmq.hpp"
#include "../perf/helpers/time.hpp"
#include "../perf/workers/raw_receiver.hpp"
#include "../perf/workers/raw_ping_pong.hpp"

void read_times_1f (perf::time_instant_t *start_time_,
    perf::time_instant_t *stop_time_)
{
    //  Load the results
    char filename [256];
    snprintf (filename, 256, "%sin.dat", PREFIX);
    FILE *input = ::fopen (filename, "r");
    assert (input);
    fscanf (input, "%llu", start_time_);
    fscanf (input, "%llu", stop_time_);
    fclose (input);
}

void read_times_2f (perf::time_instant_t *start_time_,
    perf::time_instant_t *stop_time_)
{
    //  Load the results
    char filename [256];
    snprintf (filename, 256, "%sin.dat", PREFIX);
    FILE *input = ::fopen (filename, "r");
    assert (input);
    fscanf (input, "%llu", stop_time_);
    fclose (input);
    
    snprintf (filename, 256, "%sout.dat", PREFIX);
    input = ::fopen (filename, "r");
    assert (input);
    fscanf (input, "%llu", start_time_);
    fclose (input);
}


int main (int argc, char *argv [])
{
    if (argc != 1){
        printf ("Usage: local\n");
        exit (0);
    }

    printf ("Message size: %i\n", TEST_MSG_SIZE);
    printf ("Number of messages in the throughput test: %i\n",
        TEST_MSG_COUNT_THRPUT);
    printf ("Number of roundtrips in the latency test: %i\n",
        TEST_MSG_COUNT_LATENCY);

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;
    perf::time_interval_t latency;

    {
        perf::tcp_t transport (true, "0.0.0.0", PORT_NUMBER, false);
        perf::raw_receiver_t worker (TEST_MSG_COUNT_THRPUT);
        worker.run (transport, PREFIX);
    }
    
    start_time = 0;
    stop_time = 0;
    read_times_1f (&start_time, &stop_time);
    printf ("Your average TCP throughput is %lld msgs/s\n",
        (((long long) 1000000) * ((long long) TEST_MSG_COUNT_THRPUT)) /
        (stop_time - start_time));
    
    {
        perf::zmq_t transport (true, "0.0.0.0", PORT_NUMBER);
        perf::raw_receiver_t worker (TEST_MSG_COUNT_THRPUT);
        worker.run (transport, PREFIX);
    }

    start_time = 0;
    stop_time = 0;
    read_times_1f (&start_time, &stop_time);
    printf ("Your average 0MQ throughput is %lld msgs/s\n",
        (((long long) 1000000) * ((long long) TEST_MSG_COUNT_THRPUT)) /
        (stop_time - start_time));

    {
        perf::tcp_t transport (true, "0.0.0.0", PORT_NUMBER, false);
        perf::raw_ping_pong_t worker (TEST_MSG_COUNT_LATENCY, TEST_MSG_SIZE);
        worker.run (transport, PREFIX);
    }
        
    start_time = 0;
    stop_time = 0;
    read_times_2f (&start_time, &stop_time);

    latency = (stop_time - start_time) * 1000 /
        ((long long) TEST_MSG_COUNT_LATENCY) / 2;
    printf ("Your average TCP latency is %.3f us\n",
        double (latency) / 1000);

    {
        perf::zmq_t transport (true, "0.0.0.0", PORT_NUMBER);
        perf::raw_ping_pong_t worker (TEST_MSG_COUNT_LATENCY, TEST_MSG_SIZE);
        worker.run (transport, PREFIX);
    }
        
    start_time = 0;
    stop_time = 0;
    read_times_2f (&start_time, &stop_time);

    latency = (stop_time - start_time) * 1000 /
        ((long long) TEST_MSG_COUNT_LATENCY) / 2;
    printf ("Your average 0MQ latency is %.3f us\n",
        double (latency) / 1000);

    return 0;
}

