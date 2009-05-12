/*
    Copyright (c) 2007-2009 FastMQ Inc.

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

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../../transports/zmq_transport.hpp"
#include "../scenarios/thr.hpp"
#include "../../helpers/functions.hpp"

using namespace std;

int main (int argc, char *argv [])
{
    if (argc != 6) { 
        cerr << "Usage: remote_thr <exchange interface> "
            "<queue interface> <message size> <message count> "
            "<number of threads>" << endl; 
        return 1;
    }

    //  Parse & print command line arguments.
    const char *exchange_interface = argv [1];
    const char *queue_interface = argv [2];
    size_t msg_size = atoi (argv [3]);
    int msg_count = atoi (argv [4]);
    int thread_count = atoi (argv [5]);

    cout << "threads: " << thread_count << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count: " << msg_count << endl << endl;

    //  Create *transports array.
    perf::i_transport **transports = new perf::i_transport* [thread_count];

    //  Create as many transports as threads, each worker thread uses its own
    //  names for queues and exchanges (Q0 and E0, Q1 and E1 ...)
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        //  Create queue name Q0, Q1, ...
        string queue_name ("Q");
        queue_name += perf::to_string (thread_nbr);

        //  Create exchange name E0, E1, ...
        string exchange_name ("E");
        exchange_name += perf::to_string (thread_nbr);

        //  Create zmq transport with bind = true. It means that created local 
        //  exchange will be created and binded to the global queue QX 
        //  and created local queue will be binded to global exchange EX. 
        //  Global queue and exchange have to be created before
        //  by the local_thr.
        transports [thread_nbr] = new perf::zmq_t (true,
            exchange_name.c_str (), queue_name.c_str (),
            exchange_interface, queue_interface);
    }

    //  Do the job, for more detailed info refer to ../scenarios/thr.hpp.
    perf::remote_thr (transports, msg_size, msg_count, thread_count);
   
    //  Cleanup.
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
        delete transports [thread_nbr];
    delete [] transports;

    return 0;
}

