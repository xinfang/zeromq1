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

#include <zmq/platform.hpp>
#include <zmq/stdint.hpp>
#include <zmq.hpp>
#include <zmq/wire.hpp>
using namespace zmq;

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef ZMQ_HAVE_WINDOWS
#include <sys/types.h>
#include <sys/timeb.h>
#include <Windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef ZMQ_HAVE_WINDOWS

__inline uint64_t now ()
{    
    SYSTEMTIME system_time;
    GetSystemTime (&system_time);

    FILETIME file_time;
    SystemTimeToFileTime (&system_time, &file_time);

    ULARGE_INTEGER uli;
    uli.LowPart = file_time.dwLowDateTime;
    uli.HighPart = file_time.dwHighDateTime;

    uint64_t system_time_in_ms (uli.QuadPart / 10000);
    return system_time_in_ms;
}

#else

inline uint64_t now ()
{
    struct timeval tv;
    int rc;

    rc = gettimeofday (&tv, NULL);
    assert (rc == 0);
    return tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec;
}

#endif

int main (int argc, char *argv [])
{
    //  Parse command line arguments.  
    if (argc != 5) {
        printf ("usage: send_requests <hostname> <out-interface> "
            "<sync-interface> <transaction-count>\n");
        return 1;
    }
    const char *host = argv [1];
    const char *out_interface = argv [2];
    const char *sync_interface = argv [3];
    int transaction_count = atoi (argv [4]);
    assert (transaction_count >= 1);

    //  Create the 0MQ infrastructure.
    dispatcher_t dispatcher (2);
    locator_t locator (host);
    i_thread *io = io_thread_t::create (&dispatcher);
    api_thread_t *api = api_thread_t::create (&dispatcher, &locator);

    //  Set up the wiring.
    int eid = api->create_exchange ("SEND_REQUESTS_OUT",
        scope_global, out_interface, io, 1, &io, true);
    api->create_queue ("SYNC_IN", scope_global, sync_interface, io, 1, &io);
   
    while (true) {

        //  Start after any key is hit.
        printf ("Hit ENTER to start!\n");
        getchar ();
        printf ("Running...\n");

        //  Get initial timestamp.
        uint64_t start = now ();

        //  Send messages. We don't bother to fill any data in.
        for (int counter = 0; counter != transaction_count; counter ++) {
            message_t msg (100);
            memset ((unsigned char*) msg.data (), 0, sizeof (uint64_t));  
            api->send (eid, msg);
        }

        //  Wait for synchronisation message from 'receive_replies' application.
        message_t sync;
        api->receive (&sync);

        //  Get final timestamp.
        uint64_t end = now ();

        //  Print the results.
        printf ("Throughput: %.3f transactions/second.\n\n", (double)
            (double (transaction_count * 1000000) / (end - start)));

    }

    return 0;
}
