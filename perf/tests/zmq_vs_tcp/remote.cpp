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

#define PREFIX "" 

#include "../perf/transports/tcp.hpp"
#include "../perf/transports/zmq.hpp"
#include "../perf/helpers/time.hpp"
#include "../perf/workers/raw_sender.hpp"
#include "../perf/workers/echo.hpp"

int main (int argc, char *argv [])
{
    if (argc != 2) {
        printf ("Usage: remote <ip address where \'local\' runs>\n");
        exit (0);
    }

    {
        perf::tcp_t transport (false, argv [1], PORT_NUMBER, false);
        perf::raw_sender_t worker (TEST_MSG_COUNT_THRPUT, TEST_MSG_SIZE);
        worker.run (transport, PREFIX);
    }
    printf ("TCP throughput test OK\n");

    sleep (2);  // Wait till new listener is started by the 'local'
    {
        perf::zmq_t transport (false, argv [1], PORT_NUMBER);
        perf::raw_sender_t worker (TEST_MSG_COUNT_THRPUT, TEST_MSG_SIZE);
        worker.run (transport, PREFIX);
    }
    printf ("ZMQ throughput test OK\n");

    sleep (2);  // Wait till new listener is started by the 'local'    
    {
        perf::tcp_t transport (false, argv [1], PORT_NUMBER, false);
        perf::echo_t worker (TEST_MSG_COUNT_LATENCY);
        worker.run (transport, PREFIX);
    }
    printf ("TCP latency test OK\n"); 

    sleep (2);  // Wait till new listener is started by the 'local'
    {
        perf::zmq_t transport (false, argv [1], PORT_NUMBER);
        perf::echo_t worker (TEST_MSG_COUNT_LATENCY);
        worker.run (transport, PREFIX);
    }
    printf ("ZMQ latency test OK\n"); 

    exit (0);
}

