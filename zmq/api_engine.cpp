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
    while (true) {

        int engines_alive = proxy.get_engines_alive ();

        if (engines_alive == 1 || (engines_alive != engine_count &&
              ticks == ticks_max)) {

            uint32_t signals;
            signals = engines_alive != 1 ? pollset.check () : pollset.poll ();
            
            for (int engine_nbr = 0; engine_nbr != engine_count;
                  engine_nbr ++) {
                if (signals & 0x0001)
                    proxy.revive (engine_nbr);
                signals >>= 1;
            }
        }

        current_engine = (current_engine + 1) % engine_count;
        if (proxy.read (current_engine, value_)) {
            ticks --;
            if (!ticks)
                ticks = ticks_max;
            return;
        }
    }
}

