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

#include "../../transports/zmq.hpp"
#include "../../workers/raw_ping_pong.hpp"
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"

#include "./test.hpp"

int main (int argc, char *argv [])
{
    if (argc != 3){
        printf ("Usage: local <\'global_locator\' IP> <\'global locator\' port>\n");
        return 1;
    }

    if (TEST_THREADS != 1) {
        printf ("Latency test with more than 1 thread does not nake sense.\n");
        assert (0);
    }

    //  Main results results
    std::string filename ("timing.dat");

    FILE *output = ::fopen (filename.c_str (), "w");
    assert (output);

    int msg_count;
    size_t msg_size;

    char prefix [20];
    memset (prefix, '\0', sizeof (prefix));

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;

    perf::zmq_t transport (true, "Q", "E", argv [1], atoi (argv [2]), NULL, 0);

    for (int i = 0; i < TEST_MSG_SIZE_STEPS; i++) {
    
        msg_size = TEST_MSG_SIZE_START * (0x1 << i);

        if (msg_size < SYS_BREAK) {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE * msg_size + SYS_OFF));
            msg_count /= SYS_LAT_DEN;
        } else {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE_BIG * msg_size + SYS_OFF_BIG));
            msg_count /= SYS_LAT_DEN;
        }

        printf ("Threads: %i\n", TEST_THREADS);
        printf ("Message size: %i\n", (int)msg_size);
        printf ("Number of messages in the latency test: %i\n", msg_count);

        // prefix is msgsize_threadID_, threadID is 0 (api)
        snprintf (prefix, sizeof (prefix) - 1, "%i_%i_", (int)msg_size, 0);

        {
            perf::raw_ping_pong_t worker (msg_count, msg_size);
            worker.run (transport, prefix);
        }

        printf ("Test end\n");
        fflush (stdout);

        perf::read_times_2f (&start_time, &stop_time, prefix);

        // write results to the main file
        fprintf (output, "%i %i %llu %llu\n", (int)msg_size, msg_count, start_time, stop_time);

        // delete files
        snprintf (prefix, sizeof (prefix) - 1, "%i_%i_in.dat", 
            (int)msg_size, 0);
        int rc = remove (prefix);
        assert (rc == 0);

        snprintf (prefix, sizeof (prefix) - 1, "%i_%i_out.dat", 
            (int)msg_size, 0);
        rc = remove (prefix);
        assert (rc == 0);

    }

    fclose (output);

    return 0;
}
