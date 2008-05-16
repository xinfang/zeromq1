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

#include "../../transports/zmq.hpp"
#include "../../workers/raw_sender.hpp"

#include "./test.hpp"

int main (int argc, char *argv [])
{

    if (argc != 3) {
        printf ("Usage: remote <\'global_locator\' IP> <\'global locator\' port>\n");
        return 1;
    }

    size_t msg_size;
    int msg_count;

    char *queue_name = new char [2];
    memset (queue_name, 0, 2);
    queue_name [0] = '0';

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
            perf::zmq_t transport (true, queue_name, argv [1], atoi (argv[2]), NULL, 0);
            
            perf::raw_sender_t worker (msg_count, msg_size);
            
            worker.run (transport, "");
           
         }

        queue_name [0] += 1;

        sleep (4); // Wait till new listeners are started by the 'local'

    }

    delete [] queue_name;

    return 0;
}
