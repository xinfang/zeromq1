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

#include <stdlib.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "zmq/platform.hpp"
#include "../../transports/tcp_transport.hpp"
#include "../scenarios/thr.hpp"

using namespace std;

int main (int argc, char *argv [])
{
    if (argc != 4) { 
        cerr << "Usage: remote_thr <IP address of \'local\':port> "
            << "<message size> <message count>" << endl;
        return 1;
    }
 
    //  Parse & print arguments.
    const char *peer_ip = argv [1];
    
    size_t msg_size = atoi (argv [2]);
    int msg_count = atoi (argv [3]);

    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count: " << msg_count << endl << endl;

    //  Create tcp transport.
    perf::tcp_t transport (false, peer_ip);

    //  Give time to the peer to start to listen.
#ifdef ZMQ_HAVE_WINDOWS
    Sleep (1000);
#else
    sleep (1);
#endif

    //  Do the job, for more detailed info refer to ../scenarios/thr.hpp.
    perf::remote_thr (&transport, msg_size, msg_count);
   
    return 0;
}
