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

#include "../scenarios/fi.hpp"
#include "../../transports/zmq.hpp"

using namespace std;

int main (int argc, char *argv [])
{
    if (argc != 8) {
        cerr << "Usage: local_fi <global_locator IP> <global_locator port> "
            << "<listen IP> <listen port> <message size> "
            << "<message count per publisher> <number of publishers>\n";
        return 1;
    }

    // Parse & print command line arguments
    const char *g_locator = argv [1];
    unsigned short g_locator_port = atoi (argv [2]);

    const char *listen_ip = argv [3];
    unsigned short listen_port = atoi (argv [4]);

    size_t msg_size = atoi (argv [5]);
    int msg_count = atoi (argv [6]);
    int pubs_count = atoi (argv [7]);

    cout << "publishers: " << pubs_count << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count (per publisher): " << msg_count << std::endl;

    // Create zmq transport with bind = false. It means that global queue
    // QIN and global exchange EOUT will be created without any bindings.
    perf::zmq_t transport (false, "QIN", "EOUT", g_locator, g_locator_port, 
        listen_ip, listen_port);
   
   // Do the job, for more detailed info refer to ../scenarios/fo.hpp
    perf::local_fi (&transport, msg_size, msg_count, pubs_count);

    return 0;
}
