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
    if (argc != 4) {
        printf ("usage: receive_replies <hostname> <in-interface> "
            "<reply-count>\n");
        return 1;
    }
    const char *host = argv [1];
    const char *in_interface = argv [2];
    int reply_count = atoi (argv [3]);
    assert (reply_count >= 1);

    //  Create the context for source.
    dispatcher_t dispatcher (2);
    locator_t locator (host);
    i_thread *io = io_thread_t::create (&dispatcher);
    api_thread_t *api = api_thread_t::create (&dispatcher, &locator);
      
    //  Set up the wiring.
    api->create_queue ("Q_TO_RECEIVER", scope_global, in_interface, io, 1, &io);
    
    //  Start receiving data.
    uint64_t start = 0;
    for (int counter = 0; counter != reply_count; counter ++) {
        
        //  Receive the message.
        zmq::message_t msg;
        api->receive (&msg);

        //  If it happens to contain non-zero value, it's the initial timestamp.
        if (!start)
            start = get_uint64 ((unsigned char*) msg.data ());
    }

    //  Check whether we've actually got the initial timestamp.
    assert (start != 0);

    //  Get final timestamp.  
    uint64_t end = now ();
  
    //  Print the overall processing time.
    printf ("Overall time to process %d requests was %.2f seconds\n",
        reply_count, ((double) (end - start)) / (double) 1000000);

    return 0;
}
