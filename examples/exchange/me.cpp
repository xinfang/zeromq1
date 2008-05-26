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

#include "messages.hpp"
#include "time.hpp"
#include "frequency_meter.hpp"
#include "matching_engine.hpp"
using namespace exchange;

//  Matching engine application

class me_t
{
public:

    inline me_t (const char *address, uint16_t port, const char *in_address,
          const char *out_address) :
        dispatcher (2, address, port),
        api (&dispatcher),
        pt (&dispatcher),
	orders_meter (100000, 2),
	trades_meter (100000, 3)
    {
        //  Initialise the wiring
        zmq::poll_thread_t *pt_array = {&pt};
        te_id = api.create_exchange ("TE", zmq::scope_global,
            out_address, 5556, &pt, 1, &pt_array);
        api.create_queue ("OQ", zmq::scope_global,
            in_address, 5557, &pt, 1, &pt_array);
        se_id = api.create_exchange ("SE");
        api.bind ("SE", "SQ", &pt, &pt);
    }

    void run ()
    {
        //  Message dispatch loop
        while (true) {
            void *msg = api.receive ();
            parse_message (msg, this);
            zmq::msg_dealloc (msg);
        }
    }

    inline void order (order_id_t order_id, order_type_t type,
        price_t price, volume_t volume)
    {
        orders_meter.event (this);

        //  Pass the order to matching engine
	bool trades_sent;
        if (type == ask)
            trades_sent = me.ask (this, order_id, price, volume);
        else
            trades_sent = me.bid (this, order_id, price, volume);

	//  If no trade was executed, send order confirmation
	if (!trades_sent) {
	    void *msg = make_order_confirmation (order_id);
	    api.send (te_id, msg);
	    trades_meter.event (this);
	}
    }

    inline void order_confirmation (order_id_t)
    {
        assert (false);
    }

    inline void trade (order_id_t, price_t, volume_t)
    {
        assert (false);
    }

    inline void throughput (uint8_t meter_id, uint64_t throughput)
    {
        assert (false);
    }

    inline void timestamp (uint8_t meter_id, uint64_t correlation_id,
        uint64_t timestamp)
    {
        assert (false);
    }

    inline void traded (order_id_t order_id, price_t price, volume_t volume)
    {
        //  Send trade back to the gateway
        void *msg = make_trade (order_id, price, volume);
        api.send (te_id, msg);
	trades_meter.event (this);
    }

    inline void frequency (uint8_t meter_id, uint64_t frequency)
    {
        //  Send the throughput figure to the stat component
        void *msg = make_throughput (meter_id, frequency);
        api.send (se_id, msg);
    }

private:

   matching_engine_t me;
   zmq::dispatcher_t dispatcher;
   zmq::api_engine_t api;
   zmq::poll_thread_t pt;
   int te_id;
   int se_id;
   frequency_meter_t orders_meter;
   frequency_meter_t trades_meter;
};

int main (int argc, char *argv [])
{
    if (argc != 5) {
        printf ("stat <locator address> <locator port> <in interface> "
            "<out interface>\n");
        return 1;
    }

    //  Precompute CPU frequency
    estimate_cpu_frequency ();

    //  Run the matching engine
    me_t me (argv [1], atoi (argv [2]), argv [3], argv [4]);
    me.run (); 
    return 0;
}
