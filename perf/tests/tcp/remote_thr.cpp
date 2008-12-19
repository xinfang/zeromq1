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
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include <zmq/platform.hpp>

#include "../../transports/tcp_transport.hpp"
#include "../scenarios/thr.hpp"
#include "../../helpers/functions.hpp"
#include "zmq/err.hpp"

using namespace std;

int main (int argc, char *argv [])
{
    if (argc != 5) { 
        cerr << "Usage: remote_thr <IP address of \'local\':port> "
            << "<message size> <message count> <number of threads>\n"; 
        return 1;
    }
 
    //  Parse & print arguments.
    const char *peer_ip = argv [1];
    
    //  Find port number delimiter.
    char *colon = (char *)strchr (peer_ip, ':');
    assert (colon);

    //  Parse port number.
    unsigned short listen_port = atoi (colon + 1);

    //  Cut delimiter and port number.
    *colon = 0;

    int thread_count = atoi (argv [4]);
    size_t msg_size = atoi (argv [2]);
    int msg_count = atoi (argv [3]);

    cout << "threads: " << thread_count << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "message count: " << msg_count << endl << endl;

#ifdef ZMQ_HAVE_WINDOWS

    //  Intialise Windows sockets. Note that WSAStartup can be called multiple
    //  times given that WSACleanup will be called for each WSAStartup.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    errno_assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 || HIBYTE (wsa_data.wVersion) == 2);
#endif  

    //  Create *transports array.
    perf::i_transport **transports = new perf::i_transport* [thread_count];

    //  Create as many transports as threads, each worker thread uses own
    //  transport listen port increases by 1.
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        string peer_ip_port (peer_ip);
        peer_ip_port.append (":");
        peer_ip_port.append (perf::to_string (listen_port + thread_nbr));

        //  Create tcp transport.
        transports [thread_nbr] = 
            new perf::tcp_t (false, peer_ip_port.c_str ());

        //  Give time to the peer to start to listen.
#ifdef ZMQ_HAVE_WINDOWS
        Sleep (1000);
#else
        sleep (1);
#endif
    }

    //  Do the job, for more detailed info refer to ../scenarios/thr.hpp.
    perf::remote_thr (transports, msg_size, msg_count, thread_count);
   
    //  Cleanup.
    for (int thread_nbr = 0; thread_nbr < thread_count; thread_nbr++)
    {
        delete transports [thread_nbr];
    }
    
    delete [] transports;

    return 0;
}
