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

#include "locator.hpp"
#include "dispatcher.hpp"

zmq::locator_t::locator_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    int rc = pthread_mutex_init (&sync, NULL);
    errno_assert (rc == 0);
}

zmq::locator_t::~locator_t ()
{
    int rc = pthread_mutex_destroy (&sync);
    errno_assert (rc == 0);
}

void zmq::locator_t::register_engine (i_context *context_, i_pollable *engine_)
{
    //  Enter critical section
    int rc = pthread_mutex_lock (&sync);
    errno_assert (rc == 0);

    //  Local copy of the engine list. Allows critical section to be as brief
    //  as possible.
    std::vector <engine_info_t> my_engines = engines;

    //  Register the engine in the engine repository
    engine_info_t info = {context_, engine_};
    engines.push_back (info);

    //  Leave critical section
    rc = pthread_mutex_unlock (&sync);
    errno_assert (rc == 0);

    //  Create bindings between the newly created engine and other engines
    for (std::vector <engine_info_t>::iterator it = my_engines.begin ();
          it != my_engines.end (); it ++) {

        {
            //  Create outgoing pipe
            pipe_t *pipe = new pipe_t (context_, engine_,
                it->context, it->engine);
            assert (pipe);

            //  Bind local end of the pipe
            command_t send_to;
            send_to.init_engine_send_to (NULL, pipe);
            engine_->process_command (send_to.args.engine_command.command);

            //  Bind remote end of the pipe (via admin context)
            command_t receive_from;
            receive_from.init_engine_receive_from (it->engine, pipe);
            dispatcher->send_command (it->context, receive_from);
        }

        {
            //  Create incoming pipe
            pipe_t *pipe = new pipe_t (it->context, it->engine,
                context_, engine_);
            assert (pipe);

            //  Bind local end of the pipe
            command_t receive_from;
            receive_from.init_engine_receive_from (NULL, pipe);
            engine_->process_command (receive_from.args.engine_command.command);

            //  Bind remote end of the pipe (via admin context)
            command_t send_to;
            send_to.init_engine_send_to (it->engine, pipe);
            dispatcher->send_command (it->context, send_to);
        }
    }
}
