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

    //  Register the engine with the locator
    dispatcher->get_locator ().register_engine (this, NULL);
}

zmq::api_engine_t::~api_engine_t ()
{
    //  Unregister the thread from the command dispatcher
    dispatcher->deallocate_thread_id (thread_id);
}

void zmq::api_engine_t::send (void *value_)
{
    //  Check the signals and process the commands if there are any
    ypollset_t::integer_t signals = pollset.check ();
    if (signals)
        process_commands (signals);

    //  Pass the message to the demux
    demux.instant_write (value_);
}

void *zmq::api_engine_t::receive (bool block)
{
    //  Get message from mux
    void *msg = mux.read ();

    if (block) {

        //  If there is no message, wait for signals, process commands and
        //  repeat the whole thing until there is a message.
        while (!msg) {
            ypollset_t::integer_t signals = pollset.poll ();
            assert (signals);
            process_commands (signals);
            ticks = 0;
            msg = mux.read ();
        }
    }

    //  Once every max_ticks messages check for signals and process incoming
    //  commands.
    if (++ ticks == max_ticks || !msg) {
        ypollset_t::integer_t signals = pollset.check ();
        if (signals)
            process_commands (signals);
        ticks = 0;
    }

    //  If the call is non-blocking, try to get the message once again
    //  after the commands were processed
    if (!msg)
       msg = mux.read ();

    return msg;
}

int zmq::api_engine_t::get_thread_id ()
{
    return thread_id;
}

void zmq::api_engine_t::send_command (i_context *destination_,
    const command_t &command_)
{
    dispatcher->write (thread_id, destination_->get_thread_id (), command_);
}

void zmq::api_engine_t::process_commands (ypollset_t::integer_t signals_)
{
    for (int source_thread_id = 0;
          source_thread_id != dispatcher->get_thread_count ();
          source_thread_id ++) {
        if (signals_ & (1 << source_thread_id)) {
            dispatcher_t::item_t *first;
            dispatcher_t::item_t *last;
            dispatcher->read (source_thread_id, thread_id, &first, &last);
            while (first != last) {
                switch (first->value.type) {

                //  Process engine command
                case command_t::engine_command:
                    assert (!first->value.args.engine_command.engine);
                    process_engine_command (
                        first->value.args.engine_command.command);
                    break;

                //  Unsupported/unknown command
                default:
                    assert (false);
                }
                dispatcher_t::item_t *o = first;
                first = first->next;
                delete o;
            }
        }
    }
}

void zmq::api_engine_t::process_engine_command (engine_command_t &command_)
{
    switch (command_.type) {
    case engine_command_t::revive:

        //  Forward the revive command to the pipe
        command_.args.revive.pipe->revive ();
        break;

    case engine_command_t::send_to:

        //  Start sending messages to a pipe
        demux.send_to (command_.args.send_to.pipe);
        break;

    case engine_command_t::receive_from:

        //  Start receiving messages from a pipe
        mux.receive_from (command_.args.receive_from.pipe);
        break;

    default:

        //  Unsupported/unknown command
        assert (false);
     }
}

