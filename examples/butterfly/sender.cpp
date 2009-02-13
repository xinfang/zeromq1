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

}

int main (int argc, char *argv [])
{
    const char *host;
    int roundtrip_count;
    int counter;
    uint64_t start;
    int message_size = sizeof (uint64_t);

    //  Parse command line arguments.  
    if (argc != 3) {
        printf ("usage: sender <hostname> <roundtrip-count>\n");
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

    //  Create exchange.
    int eid_sender = api_thread->create_exchange ("E_FROM_SENDER", scope, host,
        io_thread, 1, &io_thread);
   

    //  Wait till connections are accepted by the peer.  
#ifdef ZMQ_HAVE_WINDOWS
    Sleep (1000);
#else
    sleep (1);
#endif

    //  Start after any key is hit.
    printf ("Press any key to start:");
    char c[2];
    fgets (c, 2, stdin);    
    printf ("\n");
    //  Get initial timestamp.  
    start = now ();

    //  Create the first message with initial timestamp.
    zmq::message_t first_msg (sizeof (uint64_t));    
    zmq::put_uint64 ((unsigned char *) first_msg.data (), start);
    api_thread->send (eid_sender, first_msg);

    //  Start sending messages.
    for (counter = 0; counter != roundtrip_count; counter ++) {
        
        //  Create message.
        zmq::message_t msg (message_size);  

        //  Sent data.
        api_thread->send (eid_sender, msg);
    }

    //  Destroy 0MQ transport.  
    delete locator;
    delete dispatcher;
 
    return 0;
}
