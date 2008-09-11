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
#include <unistd.h>

#include "../../../czmq/czmq.h"

int main (int argc, char *argv [])
{
    const char *host;
    int message_size;
    int message_count;
    void *handle;
    int eid;
    int counter;
    void *out_buf;

    /*  Parse command line arguments.  */
    if (argc != 4) {
        printf ("usage: remote_thr <hostname> <message-size> "
            "<message-count>\n");
        return 1;
    }
    host = argv [1];
    message_size = atoi (argv [2]);
    message_count = atoi (argv [3]);

    /*  Create 0MQ transport.  */
    handle = czmq_create (host);

    /*  Create the wiring.  */
    eid = czmq_create_exchange (handle, "E", CZMQ_SCOPE_LOCAL, NULL);
    czmq_bind (handle, "E", "Q");

    /*  Create message data to send.  */
    out_buf = malloc (message_size);
    assert (out_buf);

    for (counter = 0; counter != message_count + 1; counter ++)
        czmq_send (handle, eid, out_buf, message_size, NULL);

    /*  Wait till all messages are sent.  */
    sleep (3600);

    /*  Destroy 0MQ transport.  */
    czmq_destroy (handle);

    /*  Clean up.  */
    free (out_buf);

    return 0;
}
