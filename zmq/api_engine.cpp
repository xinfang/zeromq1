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

#include "api_engine.hpp"

zmq::api_engine_t::api_engine_t (dispatcher_t *dispatcher_, int engine_id_) :
    ticks_max (100),
    ticks (1),
    proxy (dispatcher_, engine_id_)
{
    proxy.set_signaler (&pollset);
    engine_count = dispatcher_->get_engine_count ();
    current_engine = engine_count - 1;
}

void zmq::api_engine_t::send (int destination_engine_id_, const cmsg_t &value_)
{
    proxy.instant_write (destination_engine_id_, value_);
}

void zmq::api_engine_t::receive (cmsg_t *value_)
{
    //  What follows is the implementation of a "fair scheduler" (RFC970)
    //  Receive function returns messages sent by several engines.
    //  The distribution is done in round-robin manner.

    //  To save time, actual polling (time-consuming operation) is done only
    //  after each N-th message - if there are messages to process available.
    //  If there are no messages, polling is done immediately.

    while (true) {

        //  If there are no messages or if there are at least some 'dead'
        //  engines and N messages were received without polling already...
        if (!proxy.has_messages () || (proxy.is_pollable () &&
              ticks == ticks_max)) {

            //  Poll for events - either in non-blocking fashion (if there
            //  are still messages to receive available) or in blocking
            //  fashion (if there are no messages to receive).
            uint32_t signals = proxy.has_messages () ?
                pollset.check () : pollset.poll ();

            //  Check the events received and start treating the specified
            //  engines as alive.
            for (int engine_nbr = 0; engine_nbr != engine_count;
                  engine_nbr ++) {
                if (signals & 0x0001)
                    proxy.revive (engine_nbr);
                signals >>= 1;
            }
        }

        //  Move to the next engine in a round-robin
        current_engine = (current_engine + 1) % engine_count;

        //  Try to read a message. If successfull, abjust the ticks and return.
        if (proxy.read (current_engine, value_)) {
            ticks --;
            if (!ticks)
                ticks = ticks_max;
            return;
        }
    }
}

