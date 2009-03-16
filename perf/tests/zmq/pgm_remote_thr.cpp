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
#include "../../helpers/time.hpp"
#include <zmq/wire.hpp>

using namespace std;

int main (int argc, char *argv []) 
{
    if (argc != 5) {
        cerr << "Usage: pgm_remote_thr <hostname> <interface to local_exchange> " 
            << endl << "<message size> <message count>" 
            << endl;
        return 1;
    }

    //  Input arguments parsing.
    const char *host = argv [1];

    //  Global exchange name.
    const char *ex_local_name = "EX";

    const char *to_local_interface = argv [2];

    size_t msg_size = atoi (argv [3]);
    unsigned int msg_count = atoi (argv [4]);

    //  Local queue name.
    char q_name [] = "L_QUEUE";

    cout << "iface to connect to local_exchange: " << to_local_interface << endl;
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

    //  We want to receive gap notiffications.
    api->mask (zmq::message_gap);

    //  Create local queue.
    api->create_queue (q_name);

    //  Bind local queue to global exchange.
    api->bind (ex_local_name, q_name, worker, worker, to_local_interface); 

    zmq::message_t message;

    perf::time_instant_t start_time = 0;

    // Packet sequence number.
    uint32_t seq_num = 0;

    for (unsigned int i = 0; i < msg_count; i++) {
        api->receive (&message);
        if (i == 0) {
            //  Capture timestamp after first message receiving.
            start_time = perf::now ();
        }

        if (message.type () == zmq::message_gap) {
            cout << endl << "msg# "<< i << " GAP" << endl << endl;

            //  Rather assert in a case of data loss to avoid misleading
            //  throughput test results.
            assert (false);

            //  Out of data stream.
            seq_num = 0;
            continue;

        } else {

            //  Check incomming message size.
            assert (message.size () == msg_size);
        }

        //  In the beg. of the test or after data loss get seq. number 
        //  from packet.
        if (message.size () > sizeof (uint32_t) && seq_num == 0) {
            seq_num = zmq::get_uint32 ((unsigned char*) message.data ());
        } else {
            seq_num++;
        }

        //  Check seq numbers.
        if (message.size () >  sizeof (uint32_t)) {
            uint32_t current_seq = 
                zmq::get_uint32 ((unsigned char*) message.data ());
            if (seq_num != current_seq) {
                cerr << "current seq #" << current_seq 
                    << " should be #" << seq_num << endl;
                assert (false);
            }
        }

    }

    //  Capture the end timestamp of the test.
    perf::time_instant_t stop_time = perf::now ();

    //  Throughput [msg/s].
    uint64_t msg_thput = ((uint64_t) 1000000000 *
        (uint64_t) msg_count) / (uint64_t) (stop_time - start_time);

    //  Throughput [Mb/s].
    uint64_t pgm_thput = (msg_thput * msg_size * 8) /
        (uint64_t) 1000000;
                
    std::cout << "Your average throughput is " << msg_thput 
        << " [msg/s]" << std::endl;
    std::cout << "Your average throughput is " << pgm_thput 
        << " [Mb/s]" << std::endl << std::endl;

    return 0;
}
