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
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"
#include "../../workers/raw_receiver.hpp"

#include "./test.hpp"

int main (int argc, char *argv [])
{
    if (argc != 1){
        printf ("Usage: local\n");
        return 1;
    }

    //  Main results results
    std::string filename ("in.dat");

    FILE *output = ::fopen (filename.c_str (), "w");
    assert (output);

    int msg_count;
    size_t msg_size;
    char prefix [20];
    memset (prefix, '\0', sizeof (prefix));

    char file_name [255];
    memset (file_name, '\0', sizeof (file_name));

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;

    // throughput [msgs/s]
    unsigned long long msg_thput;
    // throughput [Mb/s]
    unsigned long long tcp_thput;

    for (int i = 0; i < TEST_MSG_SIZE_STEPS; i++) {
    
        msg_size = TEST_MSG_SIZE_START * (0x1 << i);

        if (msg_size < SYS_BREAK) {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE * msg_size + SYS_OFF));
            //msg_count /= TEST_THREADS;
        } else {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE_BIG * msg_size + SYS_OFF_BIG));
            //msg_count /= TEST_THREADS;
        }

//        msg_count = TEST_MSG_COUNT_THRPUT;
        printf ("Threads: %i\n", TEST_THREADS);
        printf ("Message size: %i\n", msg_size);
        printf ("Number of messages in the throughput test: %i\n", msg_count);

        // prefix is msgsize_threadID_, threadID is 0 (api)
        snprintf (prefix, sizeof (prefix) - 1, "%i_%i_", msg_size, 0);

        {
            perf::zmq_t transport (true, "0.0.0.0", PORT_NUMBER, TEST_THREADS);
            perf::raw_receiver_t worker (msg_count);
            worker.run (transport, prefix);
        }

        printf ("Test end\n");
        fflush (stdout);

        // read results from finished worker file
        snprintf (file_name, sizeof (file_name) - 1, "%i_%i_in.dat", 
            msg_size, 0);

        perf::read_times_1f (&start_time, &stop_time, file_name);

        printf ("Test time: %llu [ms]\n", (stop_time - start_time) / 
            (long long)1000);

        // throughput [msgs/s]
        msg_thput = ((long long) 1000000 * (long long) msg_count 
            ) / (stop_time - start_time);

        // throughput [Mb/s]
        tcp_thput = (msg_thput * msg_size * 8) /(long long) 1000000;
                
        printf ("Your average throughput is %llu msgs/s\n", msg_thput);
        printf ("Your average throughput is %llu Mb/s\n\n", tcp_thput);

        fprintf (output, "%i %i %llu %llu\n", msg_size, msg_count,
            start_time, stop_time);

        int rc = remove (file_name);
        assert (rc == 0);

    }
    
    fclose (output);

    return 0;
}
