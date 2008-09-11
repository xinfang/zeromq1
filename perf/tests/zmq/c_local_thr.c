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

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

#include "../../../czmq/czmq.h"

inline uint64_t now_usecs ()
{
    struct timeval tv;
    int rc;

    rc = gettimeofday (&tv, NULL);
    assert (rc == 0);
    return tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec;
}

int main (int argc, char *argv [])
{
    const char *host;
    const char *interface;
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
    interface = argv [2];
    message_size = atoi (argv [3]);
    message_count = atoi (argv [4]);

    /*  Print out the test parameters.  */
    printf ("message size: %d [B]\n", message_size);
    printf ("message count: %d\n", message_count);

    /*  Create 0MQ transport.  */
    handle = czmq_create (host);

    /*  Create the wiring.  */
    czmq_create_queue (handle, "Q", CZMQ_SCOPE_GLOBAL, interface);

    /*  Receive first message.  */
    czmq_receive (handle, &buf, &size, &ffn);
    assert (size == message_size);
    if (buf && ffn)
        ffn (buf);

    /*  Get initial timestamp.  */
    start = now_usecs ();

    for (counter = 0; counter != message_count; counter ++) {
        czmq_receive (handle, &buf, &size, &ffn);
        assert (size == message_size);
        if (buf && ffn)
            ffn (buf);
    }

    /*  Get terminal timestamp.  */
    end = now_usecs ();

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
