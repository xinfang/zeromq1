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
#include <zmq.hpp>

using namespace std;

int main (int argc, char *argv []) 
{
    if (argc != 6) {
        cerr << "Usage: pgm_remote_lat <hostname> <interface to local_exchange> " 
            << endl << "<remote_exchange network> <message size> <message count>" 
            << endl;
        return 1;
    }

    //  Input arguments parsing.
    const char *host = argv [1];

    //  Global exchange name.
    const char *ex_local_name = "EX_UP";

    const char *to_local_interface = argv [2];

    const char *ex_remote_name = "EX_DOWN";

    char network [256];
    zmq_snprintf (network, sizeof (network), "zmq.pgm://%s", argv [3]);
    network [sizeof (network) - 1] = '\0';

    size_t msg_size = atoi (argv [4]);
    int msg_count = atoi (argv [5]);

    //  Local queue name.
    char q_name [] = "L_QUEUE";

    cout << "iface to connect to local_exchange: " << to_local_interface << endl;
    cout << "remote_exchange network: " << network << endl;
    cout << "message size: " << msg_size << " [B]" << endl;
    cout << "roundtrip count: " << msg_count << endl;

    //  Create dispatcher.
    zmq::dispatcher_t dispatcher (2); 

    //  Create IO thread.
    zmq::i_thread *worker = zmq::io_thread_t::create (&dispatcher); 

    //  Create locator.
    zmq::locator_t locator (host);

    //  Create api thread.
    zmq::api_thread_t *api = zmq::api_thread_t::create (&dispatcher, &locator);

    //  Create local queue.
    api->create_queue (q_name);

    //  Bind local queue to global exchange.
    api->bind (ex_local_name, q_name, worker, worker, to_local_interface); 

    //  Create remote_exchange
    int ex_id = api->create_exchange (ex_remote_name, zmq::scope_global, 
        network, worker, 1, &worker);

    zmq::message_t message;

    for (int i = 0; i < msg_count; i++) {
        api->receive (&message);
        assert (message.size () == msg_size);
        
        api->send (ex_id, message);
    }

    //  Wait for sync message.
    api->receive (&message);

    return 0;
}
