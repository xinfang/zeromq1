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

using namespace std;

int main (int argc, char *argv []) 
{
    if (argc != 5) {
        cerr << "Usage: pgm_local_thr <hostname> <local_exchange network> " 
            << endl << "<message size> <roundtrip count>" << endl;
        cerr << "local_exchange network: iface;mcast_group:port "
            "for raw PGM" << std::endl;
        cerr << "                        udp:iface;mcast_group:port "
            "for UDP encapsulation" << std::endl;
        return 1;
    }

    //  Input arguments parsing.
    const char *host = argv [1];

    //  Global exchange name.
    const char *ex_local_name = "EX";

    char network [256];
    zmq_snprintf (network, sizeof (network), "bp/pgm://%s", argv [2]);
    network [sizeof (network) - 1] = '\0';

    size_t msg_size = atoi (argv [3]);
    int msg_count = atoi (argv [4]);

    cout << "local_exchange network: " << network << endl;
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

    cout << "Start pgm_remote_thr on remote host and "
            "pres enter to continue." << endl;

    getchar ();

    for (int i = 0; i < msg_count; i++) {
        zmq::message_t message_out (msg_size);
        api->send (ex_id, message_out);
    }

    //  Wait for a while that all messages are sent.
    sleep (1);

    return 0;
}
