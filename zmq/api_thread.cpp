/*
    Copyright (c) 2007 FastMQ Inc.

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

#include "api_thread.hpp"

zmq::api_thread_t::api_thread_t (dispatcher_t *dispatcher_, int thread_id_) :
    poll_frequency (100),
    ticks_to_poll (0),
    proxy (dispatcher_, thread_id_, &pollset)
{
    thread_count = dispatcher_->get_thread_count ();
    current_thread = thread_count - 1;
}

void zmq::api_thread_t::send (int destination_thread_id_, const cmsg_t &value_)
{
    proxy.instant_write (destination_thread_id_, value_);
}

void zmq::api_thread_t::receive (cmsg_t *value_)
{
    while (true) {

        int threads_alive = proxy.get_threads_alive ();

        if (!threads_alive || (threads_alive != thread_count &&
              ticks_to_poll == poll_frequency)) {

            uint32_t signals;
            signals = threads_alive ? pollset.check () : pollset.poll ();
            
            for (int thread_nbr = 0; thread_nbr != thread_count;
                  thread_nbr ++) {
                if (signals & 0x0001)
                    proxy.revive (thread_nbr);
                signals >>= 1;
            }
        }

        current_thread = (current_thread + 1) % thread_count;
        if (proxy.read (current_thread, value_)) {
            ticks_to_poll --;
            if (!ticks_to_poll)
                ticks_to_poll = poll_frequency;
            return;
        }
    }
}

