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

#include <iostream>
#include <cstdio>
#include <zmq.hpp>
#include "../../helpers/time.hpp"

using namespace std;

int main (int argc, char *argv []) 
{
    if (argc != 6) {
        cerr << "Usage: pgm_local_lat <hostname> <local_exchange network> " 
            << endl << "<interface to remote_exchange> <message size> " 
            << endl << "<roundtrip count>" << endl;
        cerr << "local exchange network: iface;mcast_group:port "
            "for raw PGM" << std::endl;
        cerr << "                        udp:iface;mcast_group:port "
            "for UDP encapsulation" << std::endl;
        return 1;
    }

    //  Input arguments parsing.
    const char *host = argv [1];

    //  Global exchange name.
    const char *ex_local_name = "EX_UP";

    char network [256];
    zmq_snprintf (network, sizeof (network), "bp/pgm://%s", argv [2]);
    network [sizeof (network) - 1] = '\0';

    //  Exchange name created by pgm_remote_lat.
    const char *ex_remote_name = "EX_DOWN";

    //  Network interface to use to connect to remote exchange.
    const char *to_remote_iface = argv [3];

    size_t msg_size = atoi (argv [4]);
    int msg_count = atoi (argv [5]);

    //  Local queue.
    char q_name [] = "L_QUEUE";

    cout << "local_exchange network: " << network << endl;
    cout << "iface to connect to remote_exchange: " << to_remote_iface << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "roundtrip count: " << msg_count << endl;

    //  Create dispatcher.
    zmq::dispatcher_t dispatcher (2); 

    //  Create io_thread.
    zmq::i_thread *worker = zmq::io_thread_t::create (&dispatcher); 

    //  Create locator.
    zmq::locator_t locator (host);

    //  Create api thread.
    zmq::api_thread_t *api = zmq::api_thread_t::create (&dispatcher, 
        &locator);

    //  Create global uplink exchange.
    int ex_id = api->create_exchange (ex_local_name, zmq::scope_global, 
        network, worker, 1, &worker);

    cout << "Start pgm_remote_lat on remote host and "
        "pres enter to continue." << endl;
    
    getchar (); 

    //  Bind to remote_exchange.
    api->create_queue (q_name);

    //  Bind local queue to global exchange.
    api->bind (ex_remote_name, q_name, worker, worker, to_remote_iface); 

    //  Capture timestamp at the begining of the test.
    perf::time_instant_t start_time = perf::now ();

    for (int i = 0; i < msg_count; i++) {
        zmq::message_t message_out (msg_size);
        api->send (ex_id, message_out);

        zmq::message_t message_in;
        api->receive (&message_in);

        assert (message_in.size () == msg_size);
    }

    //  Capture the end timestamp of the test.
    perf::time_instant_t stop_time = perf::now ();

    //  Send sync message that remote side can finish.
    zmq::message_t sync_message (1);
    api->send (ex_id, sync_message);

    //  Stop for a while that sync message is being send.
    sleep (1);

    //  Set 2 fixed decimal places.
    std::cout.setf(std::ios::fixed);
    std::cout.precision (2);

    //  Calculate & print results.
    uint64_t test_time = (uint64_t) (stop_time - start_time);
    double latency = (double) (test_time / 2000) / 
        (double) msg_count;

    std::cout <<  "Your average latency is " << latency 
        << " [us]" << std::endl << std::endl;

    return 0;
}
