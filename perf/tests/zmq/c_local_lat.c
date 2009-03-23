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
#include <zmq.h>

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
    double ticksDivM;
    LARGE_INTEGER ticksPerSecond;

    /*  A point in time.  */
    LARGE_INTEGER tick;

    /*  For converting tick into real time.  */
    ULARGE_INTEGER time;

    /*  Get the high resolution counter's accuracy.  */
    QueryPerformanceFrequency (&ticksPerSecond);

    /*  What time is it?  */
    QueryPerformanceCounter (&tick);

    /*  Convert the tick number into the number of seconds  */
    /*  since the system was started...  */
    ticksDivM = (double) (ticksPerSecond.QuadPart / 1000000);
    time.QuadPart = (ULONGLONG) (tick.QuadPart / ticksDivM);
         
    return time.QuadPart;
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
    const char *host;
    int message_size;
    int roundtrip_count;
    void *handle;
    int eid;
    int counter;
    void *out_buf;
    void *in_buf;
    size_t in_size;
    zmq_free_fn *in_ffn;
    uint64_t start;
    uint64_t end; 
    double latency;
        
    /*  Parse command line arguments.  */
    if (argc != 4) {
        printf ("usage: c_local_lat <hostname> <message-size> "
            "<roundtrip-count>\n");
        return 1;
    }
    host = argv [1];
    message_size = atoi (argv [2]);
    roundtrip_count = atoi (argv [3]);
    
    /*  Print out the test parameters.  */
    printf ("message size: %d [B]\n", message_size);
    printf ("roundtrip count: %d\n", roundtrip_count);

    /*  Create 0MQ transport.  */
    handle = zmq_create (host);

    /*  Create the wiring.  */
    eid = zmq_create_exchange (handle, "EL", ZMQ_SCOPE_LOCAL, NULL,
        ZMQ_STYLE_LOAD_BALANCING);
    zmq_create_queue (handle, "QL", ZMQ_SCOPE_LOCAL, NULL, -1, -1, 0);
    zmq_bind (handle, "EL", "QG", NULL, NULL);
    zmq_bind (handle, "EG", "QL", NULL, NULL);

    /*  Create message data to send.  */
    out_buf = malloc (message_size);
    assert (out_buf);

    /*  Get initial timestamp.  */
    start = now ();
    
    for (counter = 0; counter != roundtrip_count; counter ++) {
        zmq_send (handle, eid, out_buf, message_size, NULL, ZMQ_TRUE);
        zmq_receive (handle, &in_buf, &in_size, &in_ffn, NULL, ZMQ_TRUE);
        assert (in_size == message_size);
        if (in_ffn)
            in_ffn (in_buf);
    }

    /*  Get final timestamp.  */
    end = now ();

    /*  Compute and print out the latency.  */
    latency = (double) (end - start) / roundtrip_count / 2;
    printf ("Your average latency is %.2f [us]\n", latency);

    /*  Destroy 0MQ transport.  */
    zmq_destroy (handle);

    /*  Clean up.  */
    free (out_buf);

    return 0;
}
