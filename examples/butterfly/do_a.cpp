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

void do_a (int sleep_time_)
{
    //  Sleep for time sleep_time_ defined in milliseconds.
#ifdef ZMQ_HAVE_WINDOWS
    Sleep (sleep_time_);
#else
    sleep (sleep_time_ / 1000);
#endif
}

int main (int argc, char *argv [])
{
    const char *host_src;
    const char *host_dest;
    int duration;
    size_t message_size = sizeof (uint64_t);
    
    //  Parse command line arguments.  
    if (argc != 4) {
        printf ("usage: a <hostname-source> <hostname-destination>"
            "<sleep-time-milliseconds>\n");
        return 1;
    }

    host_src = argv [1];
    host_dest = argv [2];
    duration = atoi (argv [3]);
        
    //  Print out the test parameters.  
    printf ("message size: %d [B]\n", (int) message_size);
    printf ("sleep time: %d [ms]\n", duration);

    //  Create the scope.
    zmq::scope_t scope = zmq::scope_local;
   
    //  Create the context for source.
    zmq::dispatcher_t *dispatcher_src = new zmq::dispatcher_t (2);
    assert (dispatcher_src);
    zmq::locator_t *locator_src = new zmq::locator_t (host_src);
    assert (locator_src);    
    zmq::i_thread *io_thread_src = zmq::io_thread_t::create (dispatcher_src);
    assert (io_thread_src);
    zmq::api_thread_t *api_thread_src = zmq::api_thread_t::create (dispatcher_src,
        locator_src);
    assert (api_thread_src);

    //  Create queue.
    api_thread_src->create_queue ("Q_TO_A", scope, host_src,
        io_thread_src, 1, &io_thread_src, -1, -1, 0);

    //  Bind.
    api_thread_src->bind ("E_FROM_SENDER", "Q_TO_A",
        io_thread_src, io_thread_src, NULL, NULL);

    //  Create the context for destination.
    zmq::dispatcher_t *dispatcher_dest = new zmq::dispatcher_t (2);
    assert (dispatcher_dest);
    zmq::locator_t *locator_dest = new zmq::locator_t (host_dest);
    assert (locator_dest);    
    zmq::i_thread *io_thread_dest = zmq::io_thread_t::create (dispatcher_dest);
    assert (io_thread_dest);
    zmq::api_thread_t *api_thread_dest = zmq::api_thread_t::create (dispatcher_dest,
        locator_dest);
    assert (api_thread_dest);

    //  Create exchange.
    int eid_dest = api_thread_dest->create_exchange ("E_FROM_A", scope, host_dest,
        io_thread_dest, 1, &io_thread_dest);
    
    //  Bind.
    api_thread_dest->bind ("E_FROM_A", "Q_TO_QUEUE",
        io_thread_dest, io_thread_dest, NULL, NULL);
        
#ifdef ZMQ_HAVE_WINDOWS
    Sleep (1000);
#else
    sleep (1);
#endif
    
    //  Start receiving and sending messages.
    while (1) {
    
        //  Receive echoed message.
        zmq::message_t msg;
        api_thread_src->receive (&msg);

        //  Check incoming message size.
        assert (msg.size () == message_size);

        //  Do some activity A.
        do_a (duration);

        //  Send message.
        api_thread_dest->send (eid_dest, msg);
    }
                
    return 0;
}
