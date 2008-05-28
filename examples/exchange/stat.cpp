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

#include <stdio.h>
#include <queue>

#include "../../zmq/dispatcher.hpp"
#include "../../zmq/api_engine.hpp"
#include "../../zmq/poll_thread.hpp"
#include "../../zmq/msg.hpp"

#include "messages.hpp"
using namespace exchange;

//  Class to handle incoming statistics
class handler_t
{
public:

    inline handler_t ()
    {
    }

    inline void order (order_id_t order_id, order_type_t type,
        price_t price, volume_t volume)
    {
        assert (false);
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
        printf ("%1d:%08llu\n",
            (int) meter_id, throughput);
        fflush (stdout);
    }

    inline void timestamp (uint8_t meter_id, uint64_t correlation_id,
        uint64_t timestamp)
    {
        if (meter_id == 5) {
            if (!confirmation_timestamps.empty ()) {
                printf ("l:%08llu\n",
                    confirmation_timestamps.front () - timestamp);
                fflush (stdout);
                confirmation_timestamps.pop ();
            }
            else
                order_timestamps.push (timestamp);
            return;
        }

        if (meter_id == 6) {
            if (!order_timestamps.empty ()) {
                printf ("l:%08llu\n",
                    timestamp - order_timestamps.front ());
                fflush (stdout);
                order_timestamps.pop ();
            }
            else
                confirmation_timestamps.push (timestamp);
            return;
        }
    }

private:

    std::queue <uint64_t> order_timestamps;
    std::queue <uint64_t> confirmation_timestamps;
};

int main (int argc, char *argv [])
{
    if (argc != 4) {
        printf ("Usage: stat <locator address> <locator port> "
            "<statistics interface>\n");
        return 1;
    }

    //  Initialise 0MQ infrastructure
    zmq::dispatcher_t dispatcher (2, argv [1], atoi (argv [2]));
    zmq::api_engine_t api (&dispatcher);
    zmq::poll_thread_t pt (&dispatcher);
    zmq::poll_thread_t *pt_array = {&pt};

    //  Initialise the wiring
    api.create_queue ("SQ", zmq::scope_global,
        argv [3], 5558, &pt, 1, &pt_array);

    //  Handler object
    handler_t handler;

    //  Message dispatch loop
    while (true) {
        void *msg = api.receive ();
        parse_message (msg, &handler);
        zmq::msg_dealloc (msg);
    } 

    return 0;
}
