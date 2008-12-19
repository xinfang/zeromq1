/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>

#include "../../transports/qpid_transport.hpp"
#include "../scenarios/thr.hpp"
#include "../../helpers/functions.hpp"

using namespace std;

int main (int argc, char *argv [])
{
    
    if (argc != 5) {
        cerr << "Usage: remote_thr <qpid broker address> <message size> "
            <<  "<message count> <number of threads>\n";
        return 1;
    }

    //  Parse & print command line arguments.
    char *broker_address = argv [1];

    int thread_count = atoi (argv [4]);
    size_t msg_size = atoi (argv [2]);
    int msg_count = atoi (argv [3]);

    cout << "threads: " << thread_count << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count: " << msg_count << endl;

    //  Create *transports array.
    perf::i_transport **transports = new perf::i_transport* [thread_count];

    //  Create as many transports as threads, each worker thread uses own 
    //  uplink/downlonk queue.
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        string up_queue_name ("QUP");
        up_queue_name.append (perf::to_string (thread_nbr));
        
        string down_queue_name ("QDOWN");
        down_queue_name.append (perf::to_string (thread_nbr));

        //  Create tcp transport.
        transports [thread_nbr] = new perf::qpid_t (broker_address, true, 
            up_queue_name.c_str (), down_queue_name.c_str ());
    }

    //  Do the job, for more detailed info refer to ../scenarios/thr.hpp.
    perf::remote_thr (transports, msg_size, msg_count, thread_count);
    
    //  Cleanup.
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++) {
        delete transports [thread_nbr];
    }

    delete [] transports;

    return 0;
}
