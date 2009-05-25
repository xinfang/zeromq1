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

bool error_handler (const char*)
{
    //  We want this component to fail once the test is over and connections
    //  are closed.
    return false;
}

int main (int argc, char *argv [])
{    
    //  Parse command line arguments.  
    if (argc != 3) {
        printf ("usage: component1 <hostname> <processing-time [ms]>\n");
        return 1;
    }
    const char *host = argv [1];
    int processing_time = atoi (argv [2]);
        
    //  Create 0MQ infrastructure.
    dispatcher_t dispatcher (2);
    locator_t locator (host);
    set_error_handler (error_handler);
    i_thread *io = io_thread_t::create (&dispatcher);
    api_thread_t *api = api_thread_t::create (&dispatcher, &locator);

    //  Set up the wiring.
    api->create_queue ("COMPONENT1_IN");
    api->bind ("SEND_REQUESTS_OUT", "COMPONENT1_IN", io, NULL);
    int eid_dest = api->create_exchange ("COMPONENT1_OUT");
    api->bind ("COMPONENT1_OUT", "INTERMEDIATE_IN", NULL, io);
    
    //  Main event loop.
    while (true) {
        message_t msg;
        api->receive (&msg);

        //  Simulate processing, i.e. sleep for the specified time.
#ifdef ZMQ_HAVE_WINDOWS
        Sleep (processing_time);
#else
        usleep (processing_time * 1000);
#endif

        api->send (eid_dest, msg);
    }

}
