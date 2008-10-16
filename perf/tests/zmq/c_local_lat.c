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
    int message_size;
    int roundtrip_count;
    void *handle;
    int eid;
    int counter;
    void *out_buf;
    void *in_buf;
    size_t in_size;
    czmq_free_fn *in_ffn;
    uint64_t start;
    uint64_t end; 
    double latency;

    /*  Parse command line arguments.  */
    if (argc != 4) {
        printf ("usage: local_lat <hostname> <message-size> "
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
    handle = czmq_create (host);

    /*  Create the wiring.  */
    eid = czmq_create_exchange (handle, "EL", CZMQ_SCOPE_LOCAL, NULL);
    czmq_create_queue (handle, "QL", CZMQ_SCOPE_LOCAL, NULL);
    czmq_bind (handle, "EL", "QG");
    czmq_bind (handle, "EG", "QL");

    /*  Create message data to send.  */
    out_buf = malloc (message_size);
    assert (out_buf);

    /*  Get initial timestamp.  */
    start = now_usecs ();
    
    for (counter = 0; counter != roundtrip_count; counter ++) {
        czmq_send (handle, eid, out_buf, message_size, NULL);
        czmq_receive (handle, &in_buf, &in_size, &in_ffn);
        assert (in_size == message_size);
        if (in_ffn)
            in_ffn (in_buf);
    }

    /*  Get final timestamp.  */
    end = now_usecs ();

    /*  Compute and print out the latency.  */
    latency = (double) (end - start) / roundtrip_count / 2;
    printf ("Your average latency is %.2f [us]\n", latency);

    /*  Destroy 0MQ transport.  */
    czmq_destroy (handle);

    /*  Clean up.  */
    free (out_buf);

    return 0;
}
