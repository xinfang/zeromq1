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

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../../transports/zmq.hpp"
#include "../scenarios/thr_lat.hpp"

using namespace std;

int main (int argc, char *argv [])
{

    if (argc != 6) { 
        cerr << "Usage: remote_thr <global_locator IP> <global_locator port> "
            << "<message size> <message count> <msgs per sec>\n"; 
        return 1;
    }

    const char *g_locator = argv [1];
    unsigned short g_locator_port = atoi (argv [2]);

    size_t msg_size = atoi (argv [3]);
    int msg_count = atoi (argv [4]);
    int msgs_per_sec = atoi (argv [5]);

    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count: " << msg_count << endl;
    cout << "messages per second: " << msgs_per_sec << " [msg/s]" << endl;


    perf::zmq_t transport  (true, "Q0", "E0", g_locator, g_locator_port, 
        NULL, 0);

    perf::remote_thr_lat (&transport, msg_size, msg_count, msgs_per_sec);
    
    return 0;
}

