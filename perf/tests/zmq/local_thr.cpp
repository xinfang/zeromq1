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
#include "../../workers/raw_sender.hpp"

#include "./test.hpp"

int main (int argc, char *argv [])
{
    if (argc != 5){
        printf ("Usage: local <listen IP> <listen port> <\'global_locator\' IP> <\'global locator\' port>\n");
        return 1;
    }

    FILE *output = ::fopen ("timing.dat", "w");
    assert (output);

    int msg_count;
    size_t msg_size;

/*
    char *queue_name = new char [2];
    memset (queue_name, 0, 2);
    queue_name [0] = '0';
*/
    char file_name [255];
    memset (file_name, '\0', sizeof (file_name));

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;

    // throughput [msgs/s]
    unsigned long long msg_thput;
    // throughput [Mb/s]
    unsigned long long tcp_thput;
    printf ("receiver: listen %s:%i, GL %s:%i\n", argv [1], 
        atoi (argv [2]), argv [3], atoi (argv [4]));

    // Q0 message queue, ES sync exhange
    perf::zmq_t transport (false, "Q0", "ES", argv [3], atoi (argv [4]), argv [1], atoi (argv [2]));

    for (int i = 0; i < TEST_MSG_SIZE_STEPS; i++) {
   
        msg_size = TEST_MSG_SIZE_START * (0x1 << i);

        if (msg_size < SYS_BREAK) {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE * msg_size + SYS_OFF));
        } else {
            msg_count = (int)((TEST_TIME * 100000) / 
                (SYS_SLOPE_BIG * msg_size + SYS_OFF_BIG));
        }

        printf ("Message size: %i\n", (int)msg_size);
        printf ("Number of messages in the throughput test: %i\n", msg_count);

        { 
            perf::raw_receiver_t worker (msg_count, msg_size);
    
            snprintf (file_name, sizeof (file_name) - 1, "%i_%i_", (int)msg_size, 0); 

            worker.run (transport, file_name);

            printf ("received %i msgs, sending sync message\n", msg_count);
            
            // sync message 1B
            transport.send_sync_message ();
        }
         
        // read results from finished worker file
        snprintf (file_name, sizeof (file_name) - 1, "%i_%i_in.dat", (int)msg_size, 0);
        perf::read_times_1f (&start_time, &stop_time, file_name);

        fprintf (output, "%i %i %llu %llu\n", (int)msg_size, 
              msg_count, start_time, stop_time);

        // delete file
        int rc = remove (file_name);
        assert (rc == 0);

        printf ("Test time: %llu [ms]\n", (stop_time - start_time) / (long long)1000);

        // throughput [msgs/s]
        msg_thput = ((long long) 1000000 * (long long) msg_count) / (stop_time - start_time);

        // throughput [Mb/s]
        tcp_thput = (msg_thput * msg_size * 8) /(long long) 1000000;
               
        printf ("Your average throughput is %llu msgs/s\n", msg_thput);
        printf ("Your average throughput is %llu Mb/s\n\n", tcp_thput);

    }
    
    printf ("Test end\n");
    fflush (stdout);

    fclose (output);

    return 0;
}

