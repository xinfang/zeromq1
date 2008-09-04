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

#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "../../transports/openamq.hpp"
#include "../scenarios/thr.hpp"
#include "../../helpers/functions.hpp"

using namespace std;

int main (int argc, char *argv [])
{
    if (argc != 5) {
        cerr << "Usage: local_thr <hostname> <message size> "
            "<message count> <number of threads>" << endl;
        return 1;
    }

    //  Parse & print command line arguments.
    const char *host = argv [1];
    size_t msg_size = atoi (argv [2]);
    int msg_count = atoi (argv [3]);
    int thread_count = atoi (argv [4]);

    cout << "threads: " << thread_count << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count: " << msg_count << endl;

    //  Create *transports array.
    perf::i_transport **transports = new perf::i_transport* [thread_count];

    //  Create as many transports as threads.
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++) {

        //  Create send routing key.
        string srk ("srk");
        srk += perf::to_string (thread_nbr);

        //  Create receive routing key.
        string rrk ("rrk");
        rrk += perf::to_string (thread_nbr);

        //  Create OpenAMQ transport.
        transports [thread_nbr] = new perf::openamq_t (host,
            srk.c_str (), rrk.c_str (), true);
    }

    //  Do the job, for more detailed info refer to ../scenarios/thr.hpp.
    perf::local_thr (transports, msg_size, msg_count, thread_count);
    
    //  Cleanup.
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
        delete transports [thread_nbr];    
    delete [] transports;

    return 0;
}
