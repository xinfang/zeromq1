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

#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "../../transports/zmq.hpp"
#include "../scenarios/thr.hpp"
#include "../../helpers/functions.hpp"

using namespace std;

int main (int argc, char *argv [])
{
    if (argc != 8){
        cerr << "Usage: local_thr <global_locator IP> <global_locator port> "
            << "<listen IP> <listen port> <message size> <message count> "
            << "<number of threads>\n";
        return 1;
    }

    // Parse & print command line arguments
    const char *g_locator = argv [1];
    unsigned short g_locator_port = atoi (argv [2]);

    const char *listen_ip = argv [3];
    unsigned short listen_port = atoi (argv [4]);

    int thread_count = atoi (argv [7]);
    size_t msg_size = atoi (argv [5]);
    int msg_count = atoi (argv [6]);

    cout << "threads: " << thread_count << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count: " << msg_count << endl;

    // Create *transports array
    perf::i_transport **transports = new perf::i_transport* [thread_count];

    // Create as many transports as threads, each worker thread uses own transport 
    // names for queues and exchanges are Q0 and E0, Q1 and E1 ...
    // listen port increased by 2 (first port usese queue, 
    // second port uses exchange)
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        // Create queue name Q0, Q1, ...
        string queue_name ("Q");
        queue_name += perf::to_string (thread_nbr);

        // Create exchange name E0, E1, ...
        string exchange_name ("E");
        exchange_name += perf::to_string (thread_nbr);

        // Create zmq transport with bind = false. It means that global queue
        // QX and global exchange EX will be created without any bindings.
        transports [thread_nbr] = new perf::zmq_t (false, queue_name.c_str (), 
            exchange_name.c_str (), g_locator, g_locator_port, 
            listen_ip, listen_port + 2 * thread_nbr);
    }

    // Do the job, for more detailed info refer to ../scenarios/thr.hpp
    perf::local_thr (transports, msg_size, msg_count, thread_count);
    
    // Cleanup
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        delete transports [thread_nbr];
    }
    
    delete [] transports;

    return 0;
}
