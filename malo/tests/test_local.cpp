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

//#include <cstdlib>
//#include <cstdio>
#include <string>
//#include <limits>

#include "../../transports/zmq.hpp"
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"
#include "../../workers/raw_ping_pong.hpp"

//#include "./test.hpp"

using namespace std;

void *worker_function (void *);

int main (int argc, char *argv [])
{
    if (argc != 1){
        printf ("Usage: local\n");
        exit (0);
    }

    std::string filename ("timing.dat");

    FILE *output = ::fopen (filename.c_str (), "w");
    assert (output);

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;

    perf::zmq_t transport (true, "0.0.0.0", 5555, 1);
    perf::raw_ping_pong_t worker (50000, 6);
    worker.run (transport, "6_0_", 0);

    perf::read_times_2f (&start_time, &stop_time, "6_0_");

    fprintf (output, "%i %i %llu %llu\n", 6, 
            50000, start_time, stop_time);

    return 0;
}

