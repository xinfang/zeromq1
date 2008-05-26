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

#include "../../zmq/dispatcher.hpp"
#include "../../zmq/api_engine.hpp"
#include "../../zmq/poll_thread.hpp"
#include "../../zmq/msg.hpp"

#include "ticker.hpp"
#include "messages.hpp"
#include "frequency_meter.hpp"
using namespace exchange;

//  Sender half of the gateway application
//  i.e. Automated order feeder sending orders to the matching engine

class sender_t
{
public:

    inline sender_t (zmq::dispatcher_t *dispatcher) :
        api (dispatcher),
        pt (dispatcher),
        meter (200000, 1)
    {
        //  Initialise the wiring
        oe_id = api.create_exchange ("OE");
        api.bind ("OE", "OQ", &pt, &pt);
        se_id = api.create_exchange ("SE");
        api.bind ("SE", "SQ", &pt, &pt);
    }

    void run (uint64_t frequency, uint64_t batch_size)
    {
        //  Initialise the ticker
        ticker_t ticker (frequency / batch_size);

        order_id_t order_id = 0;
        while (true) {

            //  Delay a bit to get stream of orders with stable throughput
            ticker.wait_for_tick ();

            for (int counter = 0; counter != batch_size; counter ++) {

                //  Create random order
                order_id ++;
                order_type_t type = (random () % 2) ? ask : bid;
                price_t price = random () % 100 + 450;
                volume_t volume = random () % 100 + 1;

                //  Send the order to the matching engine
                void *msg = make_order (order_id, type, price, volume);
                api.presend (oe_id, msg);
                meter.event (this);

                //  Send a timestamp to the stat component
                if (order_id % 200000 == 0) {
                    void *msg = make_timestamp (5, order_id, now_usec ());
                    api.presend (se_id, msg);
                }
            }

	    //  Flush the batch to the network
	    api.flush ();
        }
    }

    inline void throughput (uint8_t meter_id, uint64_t frequency)
    {
        assert (false);
    }

    inline void frequency (uint8_t meter_id, uint64_t frequency)
    {
        //  Send the throughput figure to the stat component
        void *msg = make_throughput (meter_id, frequency);
        api.send (se_id, msg);
    }

private:

    //  0MQ infrastructure
    zmq::api_engine_t api;
    zmq::poll_thread_t pt;

    //  Exchange IDs
    int oe_id;
    int se_id;

    //  Measuring the rate of outgoing messages (orders)
    frequency_meter_t meter;
};

//  Receiver half of the gateway application

class receiver_t
{
public:

    receiver_t (zmq::dispatcher_t *dispatcher) :
        api (dispatcher),
        pt (dispatcher),
        meter (200000, 4),
        last_timestamp (0)
    {
        //  Initialise the wiring
        api.create_queue ("TQ");
        api.bind ("TE", "TQ", &pt, &pt);
        se_id = api.create_exchange ("SE");
        api.bind ("SE", "SQ", &pt, &pt);
    }

    void run ()
    {
        //  Main message dispatch loop
        while (true) {
            void *msg = api.receive ();
            parse_message (msg, this);
            zmq::msg_dealloc (msg);
        }
    }

    inline void order (order_id_t order_id, order_type_t type,
        price_t price, volume_t volume)
    {
        assert (false);
    }

    inline void order_confirmation (order_id_t order_id)
    {
        //  Measuring throughput
        meter.event (this);

        //  Taking timestamps
        if (order_id % 200000 == 0 && order_id > last_timestamp) {
            void *msg = make_timestamp (6, order_id, now_usec ());
            api.send (se_id, msg);
            last_timestamp = order_id;
        }
    }

    inline void trade (order_id_t order_id, price_t price, volume_t volume)
    {
        //  As we are doing to business logic with trades and
        //  order confirmations we can handle them in the same manner.
        order_confirmation (order_id);
    }

    inline void throughput (uint8_t meter_id, uint64_t frequency)
    {
        assert (false);
    }

    inline void timestamp (uint8_t meter_id, uint64_t correlation_id,
        uint64_t frequency)
    {
        assert (false);
    }

    inline void frequency (uint8_t meter_id, uint64_t frequency)
    {
        //  Send the throughput figure to the stat component
        void *msg = make_throughput (meter_id, frequency);
        api.send (se_id, msg);
    }

private:

    //  0MQ infrastructure
    zmq::api_engine_t api;
    zmq::poll_thread_t pt;

    //  Exchange IDs
    int se_id;

    //  Measuring the rate of incoming messages (trades & order confirmations)
    frequency_meter_t meter;

    //  Order ID for which last timestamp was taken
    order_id_t last_timestamp;
};

//  Main routine for the sender thread
void *sender_routine (void *arg)
{
    //  Start the sender with the rate of X orders per second
    zmq::dispatcher_t *dispatcher = (zmq::dispatcher_t*) arg;
    sender_t sender (dispatcher);
    sender.run (450000, 20);
}

int main (int argc, char *argv [])
{
    if (argc != 3) {
        printf ("stat <locator address> <locator port>\n");
        return 1;
    }

    //  Precompute the CPU frequency
    estimate_cpu_frequency ();

    //  Create the shared message dispatcher
    zmq::dispatcher_t dispatcher (4, argv [1], atoi (argv [2]));

    //  Run the sender thread
    pthread_t sender_thread;
    int rc = pthread_create (&sender_thread, NULL, sender_routine, &dispatcher);
    assert (rc == 0);

    //  Run the receiving loop
    receiver_t receiver (&dispatcher);
    receiver.run ();

    return 0;
}
