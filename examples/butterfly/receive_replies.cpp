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
    GetSystemTime (&system_time );

    FILETIME file_time;
    SystemTimeToFileTime (&system_time, &file_time);

    ULARGE_INTEGER uli;
    uli.LowPart = file_time.dwLowDateTime;
    uli.HighPart = file_time.dwHighDateTime;

    uint64_t system_time_in_ms( uli.QuadPart/10000 );
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

void free_fn (void *data) 
{
    free (data);
}

int main (int argc, char *argv [])
{
    const char *host;
    int roundtrip_count;
    uint64_t start;
    uint64_t end; 
    double latency;
    int counter;
    int message_size = sizeof (uint64_t);

    //  Parse command line arguments.  
    if (argc != 3) {
        printf ("usage: receiver <hostname> <roundtrip-count>\n");
        return 1;
    }
    host = argv [1];
    roundtrip_count = atoi (argv [2]);
    
    //  Print out the test parameters.  
    printf ("message size: %d [B]\n", message_size);
    printf ("roundtrip count: %d\n", roundtrip_count);

    //  Create the scope.
    zmq::scope_t scope = zmq::scope_global;

    //  Create the context for source.
    zmq::dispatcher_t *dispatcher = new zmq::dispatcher_t (2);
    assert (dispatcher);
    zmq::locator_t *locator = new zmq::locator_t (host);
    assert (locator);    
    zmq::i_thread *io_thread = zmq::io_thread_t::create (dispatcher);
    assert (io_thread);
    zmq::api_thread_t *api_thread = zmq::api_thread_t::create (dispatcher,
        locator);
    assert (api_thread);
      
    //  Create queue.
    api_thread->create_queue ("Q_TO_RECEIVER", scope, host,
        io_thread, 1, &io_thread, -1, -1, 0);

    //  Wait till both connection are accepted by the peer.  
#ifdef ZMQ_HAVE_WINDOWS
    Sleep (1000);
#else
    sleep (1);
#endif
    
    zmq::message_t msg;

    //  Receive the first message and extract start time.
    api_thread->receive (&msg);
    start = zmq::get_uint64 ((unsigned char *) msg.data());
    
    //  Start receiving data.
    for (counter = 0; counter != roundtrip_count; counter ++) {
        
        //  Receive data.
        api_thread->receive (&msg);
    }

    //  Get final timestamp.  
    end = now ();
  
    //  Compute and print out the latency.  
    latency = (double) (end - start) / roundtrip_count / 2;
    printf ("Your average latency is %.2f [ms]\n", latency);

    //  Destroy 0MQ transport.  
    delete locator;
    delete dispatcher;
  
    return 0;
}
