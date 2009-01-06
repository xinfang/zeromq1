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

#include <zmq/platform.hpp>
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
#endif

#ifdef ZMQ_HAVE_SOLARIS
#include <inttypes.h>
#elif defined ZMQ_HAVE_WINDOWS

typedef __int8 uint8_t;
typedef __int16 uint16_t;
typedef __int32 uint32_t;
typedef __int64 uint64_t;

#else
#include <stdint.h>
#endif

#include <zmq/czmq.h>

#ifdef ZMQ_HAVE_WINDOWS

    __inline uint64_t now () {
        
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
        ticksDivM = (double) (ticksPerSecond.QuadPart / 1000000000);
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
    const char *iface;
    int message_size;
    int message_count;
    void *handle;
    int counter;
    void *buf;
    size_t size;
    czmq_free_fn *ffn;
    uint64_t start;
    uint64_t end;
    uint64_t message_throughput;
    uint64_t megabit_throughput;

    /*  Parse command line arguments.  */
    if (argc != 5) {
        printf ("usage: local_thr <hostname> <interface> "
            "<message-size> <message-count>\n");
        return 1;
    }
    host = argv [1];
    iface = argv [2];
    message_size = atoi (argv [3]);
    message_count = atoi (argv [4]);

    /*  Print out the test parameters.  */
    printf ("message size: %d [B]\n", message_size);
    printf ("message count: %d\n", message_count);

    /*  Create 0MQ transport.  */
    handle = czmq_create (host);

    /*  Create the wiring.  */
    czmq_create_queue (handle, "Q", CZMQ_SCOPE_GLOBAL, iface);

    /*  Receive first message.  */
    czmq_receive (handle, &buf, &size, &ffn);
    assert (size == message_size);
    if (buf && ffn)
        ffn (buf);

    /*  Get initial timestamp.  */
    start = now ();

    for (counter = 0; counter != message_count; counter ++) {
        czmq_receive (handle, &buf, &size, &ffn);
        assert (size == message_size);
        if (buf && ffn)
            ffn (buf);
    }

    /*  Get terminal timestamp.  */
    end = now ();

    /*  Compute and print out the throughput.  */
    message_throughput = 1000000 * (uint64_t) message_count /
        (end - start);
    megabit_throughput = message_throughput * message_size * 8 /
        1000000;
    printf ("Your average throughput is %ld [msg/s]\n",
        (long) message_throughput);
    printf ("Your average throughput is %ld [Mb/s]\n",
        (long) megabit_throughput);

    /*  Destroy 0MQ transport.  */
    czmq_destroy (handle);

    return 0;
}
