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
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef ZMQ_HAVE_WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <zmq.h>

int main (int argc, char *argv [])
{
    const char *host;
    const char *in_interface;
    const char *out_interface;
    int message_size;
    int roundtrip_count;
    void *handle;
    int eid;
    int counter;
    void *buf;
    size_t size;
    zmq_free_fn *ffn;
    
    /*  Parse command line arguments.  */
    if (argc != 6) {
        printf ("usage: c_remote_lat <hostname> <in-interface> <out-interface> "
            "<message-size> <roundtrip-count>\n");
        return 1;
    }
    host = argv [1];
    in_interface = argv [2];
    out_interface = argv [3];
    message_size = atoi (argv [4]);
    roundtrip_count = atoi (argv [5]);
    
     /*  Create 0MQ transport.  */
    handle = zmq_create (host);

    /*  Create the wiring.  */
    eid = zmq_create_exchange (handle, "EG", ZMQ_SCOPE_GLOBAL, out_interface,
        ZMQ_STYLE_LOAD_BALANCING);
    zmq_create_queue (handle, "QG", ZMQ_SCOPE_GLOBAL, in_interface, -1, -1, 0);

    for (counter = 0; counter != roundtrip_count; counter ++) {
        zmq_receive (handle, &buf, &size, &ffn, NULL, ZMQ_TRUE);
        assert (size == message_size);
        zmq_send (handle, eid, buf, size, ffn, ZMQ_TRUE);
    }
#ifdef ZMQ_HAVE_WINDOWS
    Sleep (2000);
#else
    sleep (2);
#endif

    /*  Destroy 0MQ transport.  */
    zmq_destroy (handle);

    return 0;
}
