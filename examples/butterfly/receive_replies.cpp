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

int main (int argc, char *argv [])
{
    //  Parse command line arguments.  
    if (argc != 4) {
        printf ("usage: receive_replies <hostname> <in-interface> "
            "<transaction-count>\n");
        return 1;
    }
    const char *host = argv [1];
    const char *in_interface = argv [2];
    int transaction_count = atoi (argv [3]);
    assert (transaction_count >= 1);

    //  Create 0MQ infrastructure.
    dispatcher_t dispatcher (2);
    locator_t locator (host);
    i_thread *io = io_thread_t::create (&dispatcher);
    api_thread_t *api = api_thread_t::create (&dispatcher, &locator);
      
    //  Set up the wiring.
    api->create_queue ("RECEIVE_REPLIES_IN", scope_global, in_interface,
        io, 1, &io);
    int eid = api->create_exchange ("SYNC_OUT");
    api->bind ("SYNC_OUT", "SYNC_IN", NULL, io);
    
    while (true) {

        //  Wait for all the replies.
        for (int counter = 0; counter != transaction_count; counter ++) {
            message_t msg;
            api->receive (&msg);
        }

        //  Send the synchronisation messages to 'send_requests' application.
        message_t sync (1);
        api->send (eid, sync);
    }
}
