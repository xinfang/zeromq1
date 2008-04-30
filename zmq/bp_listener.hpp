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

#ifndef __ZMQ_BP_LISTENER_HPP_INCLUDED__
#define __ZMQ_BP_LISTENER_HPP_INCLUDED__

#include "i_pollable.hpp"
#include "poll_thread.hpp"

namespace zmq
{

    class bp_listener_t : public i_pollable
    {
    public:

        //  Creates a BP listener. handler_thread argument points
        //  to the thread that will serve newly-created BP engines.
        static bp_listener_t *create (poll_thread_t *thread_,
            const char *interface_, uint16_t port_,
            poll_thread_t *handler_thread_);

        //  i_pollable implementation
        int get_fd ();
        short get_events ();
        void in_event ();
        void out_event ();
        void process_command (const engine_command_t &command_);

    private:

        bp_listener_t (poll_thread_t *thread_, const char *interface_,
            uint16_t port_, poll_thread_t *handler_thread_);
        ~bp_listener_t ();

        //  The context listener is running in
        i_context *context;

        //  The thread to manage newly-created BP engines
        poll_thread_t *handler_thread;

        //  Listening socket
        int sock;
    };

}

#endif
