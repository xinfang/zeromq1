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
#include "../../zmq/locator.hpp"
#include "../../zmq/api_engine.hpp"
#include "../../zmq/poll_thread.hpp"
#include "../../zmq/message.hpp"

#include "../../perf/helpers/time.hpp"
using namespace perf;

#include "messages.hpp"
#include "frequency_meter.hpp"
#include "matching_engine.hpp"
using namespace exchange;

//  Matching engine application

class me_t
{
public:

    inline me_t (const char *address, uint16_t port, const char *in_address,
          const char *out_address) :
        dispatcher (3),
        locator (address, port),
        api (&dispatcher, &locator),
        pt_in (&dispatcher),
        pt_out (&dispatcher),
	in_meter (500000, 2),
	out_meter (500000, 3)
    {
        //  Initialise the wiring
        zmq::poll_thread_t *pt_out_array = {&pt_out};
        te_id = api.create_exchange ("TE", zmq::scope_global,
            out_address, 5556, &pt_out, 1, &pt_out_array);
        zmq::poll_thread_t *pt_in_array = {&pt_in};
        api.create_queue ("OQ", zmq::scope_global,
            in_address, 5557, &pt_in, 1, &pt_in_array);
        se_id = api.create_exchange ("SE");
        bool rc = api.bind ("SE", "SQ", &pt_out, &pt_out);
        assert (rc);
    }

    void run ()
    {
        //  Message dispatch loop
        while (true) {
            zmq::message_t msg;
            api.receive (&msg);
            parse_message (&msg, this);
        }
    }

    inline void order (order_id_t order_id, order_type_t type,
        price_t price, volume_t volume)
    {
        in_meter.event (this);

        //  Pass the order to matching engine
	bool trades_sent;
        if (type == ask)
            trades_sent = me.ask (this, order_id, price, volume);
        else
            trades_sent = me.bid (this, order_id, price, volume);

	//  If no trade was executed, send order confirmation
	if (!trades_sent) {
            zmq::message_t msg;
	    make_order_confirmation (order_id, &msg);
	    api.presend (te_id, &msg);
	    out_meter.event (this);
	}

	//  Flush the outgoing messages
        api.flush ();
    }

    inline void order_confirmation (order_id_t)
    {
        assert (false);
    }

    inline void trade (order_id_t, price_t, volume_t)
    {
        assert (false);
    }

    inline void quote (price_t bid, price_t ask)
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
        zmq::message_t msg;
        make_trade (order_id, price, volume, &msg);
        api.presend (te_id, &msg);
	out_meter.event (this);
    }

    inline void quoted (price_t bid, price_t ask)
    {
        //  Send quote back to the gateway
        zmq::message_t msg;
        make_quote (ask, bid, &msg);
        api.presend (te_id, &msg);
	out_meter.event (this);
    }

    inline void frequency (uint8_t meter_id, uint64_t frequency)
    {
        //  Send the throughput figure to the stat component
        zmq::message_t msg;
        make_throughput (meter_id, frequency, &msg);
        api.presend (se_id, &msg);
    }

private:

   matching_engine_t me;
   zmq::dispatcher_t dispatcher;
   zmq::locator_t locator;
   zmq::api_engine_t api;
   zmq::poll_thread_t pt_in;
   zmq::poll_thread_t pt_out;
   int te_id;
   int se_id;
   frequency_meter_t in_meter;
   frequency_meter_t out_meter;
};

int main (int argc, char *argv [])
{
    if (argc != 5) {
        printf ("Usage: me <locator address> <locator port> <in interface> "
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
