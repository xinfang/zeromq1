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

zmq::api_engine_t::api_engine_t (dispatcher_t *dispatcher_) :
    ticks (0),
    dispatcher (dispatcher_)
{
    //  Register the thread with the command dispatcher
    thread_id = dispatcher->allocate_thread_id (&pollset);
}

zmq::api_engine_t::~api_engine_t ()
{
    //  Unregister the thread from the command dispatcher
    dispatcher->deallocate_thread_id (thread_id);
}

void zmq::api_engine_t::send (void *value_)
{
    demux.instant_write (value_);
}

void zmq::api_engine_t::receive (void **value_)
{
    //  Get message from mux
    bool ok = mux.read (value_);

    //  If there is no message, wait for signals, process commands and
    //  repeat the whole thing until there is a message.
    while (!ok) {
        ypollset_t::integer_t signals = pollset.poll ();
        assert (signals);
        process_commands (signals);
        ticks = 0;
        ok = mux.read (value_);
    }

    //  Once every max_ticks messages check for signals and process incoming
    //  commands.
    if (++ ticks == max_ticks) {
        ypollset_t::integer_t signals = pollset.check ();
        if (signals)
            process_commands (signals);
        ticks = 0;
    }
}

void zmq::api_engine_t::process_commands (ypollset_t::integer_t signals)
{
    //  TODO
    assert (false);
}

