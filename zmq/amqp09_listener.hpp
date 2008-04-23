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

#ifndef __ZMQ_AMQP09_LISTENER_HPP_INCLUDED__
#define __ZMQ_AMQP09_LISTENER_HPP_INCLUDED__

#include "i_pollable.hpp"
#include "i_thread.hpp"

namespace zmq
{

    class amqp09_listener_t : public i_pollable
    {
    public:

        //  Creates a AMQP listener. handler_thread argument points
        //  to the thread that will serve newly-created AMQP engines.
        static amqp09_listener_t *create (i_thread *handler_thread_,
            const char *interface_, uint16_t port_);

        //  i_pollable implementation
        void set_thread (i_thread *thread_);
        int get_fd ();
        short get_events ();
        void in_event ();
        void out_event ();
        void process_command (const engine_command_t &command_);

    private:

        amqp09_listener_t (i_thread *handler_thread_,
            const char *interface_, uint16_t port_);
        ~amqp09_listener_t ();

        //  The thread managing the listener
        i_thread *thread;

        //  The thread to manage newly-created BP engines
        i_thread *handler_thread;

        //  Listening socket
        int sock;
    };

}

#endif
