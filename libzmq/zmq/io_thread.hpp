/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_IO_THREAD_HPP_INCLUDED__
#define __ZMQ_IO_THREAD_HPP_INCLUDED__

#include <vector>

#include <zmq/i_thread.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/ysocketpair.hpp>
#include <zmq/thread.hpp>
#include <zmq/fd.hpp>

namespace zmq
{

    //  Generic part of the I/O thread. Polling-mechanism-specific features
    //  are implemented in separate "polling objects".

    class io_thread_t : public i_thread, public i_pollable
    {
    public:

        static i_thread *create (dispatcher_t *dispatcher_);
        
        //  i_thread implementation.
        int get_thread_id ();
        void send_command (i_thread *destination_, const command_t &command_);
        void stop ();
        void destroy ();

        //  i_pollable implementation.
        void register_event (i_poller *poller_);
        void in_event ();
        void out_event ();
        void timer_event ();
        void unregister_event ();

    private:

        io_thread_t (dispatcher_t *dispatcher_);
        ~io_thread_t ();

        //  Processes individual command.
        void process_command (const command_t &command_);

        //  Pointer to dispatcher.
        dispatcher_t *dispatcher;

        //  Thread ID allocated for the poll thread by dispatcher.
        int thread_id;

        //  Poll thread gets notifications about incoming commands using
        //  this socketpair.
        ysocketpair_t signaler;

        //  Handle associated with signaler's file descriptor.
        handle_t signaler_handle;

        //  We perform I/O multiplexing using a poller object.
        i_poller *poller;

        //  List of all registered engines.
        typedef std::vector <i_engine*> engines_t;
        engines_t engines;
    };

}

#endif
