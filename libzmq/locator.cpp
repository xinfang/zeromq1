/*
    Copyright (c) 2007-2008 FastMQ Inc.

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

#include <assert.h>
#include <string.h>

#include <zmq/err.hpp>
#include <zmq/locator.hpp>
#include <zmq/pollable_factory.hpp>
#include <zmq/config.hpp>
#include <zmq/formatting.hpp>

zmq::locator_t::locator_t (const char *hostname_)
{
    if (hostname_) {

        //  If port number is not explicitly specified, use the default one.
        if (!strchr (hostname_, ':')) {
            char buf [256];
            zmq_snprintf (buf, 256, "%s:%d", hostname_,
                (int) default_locator_port);
            hostname_ = buf;
        }

        //  Open connection to global locator.
        global_locator = new tcp_socket_t (hostname_, true);
    }
    else
        global_locator = NULL;
}

zmq::locator_t::~locator_t ()
{
    if (global_locator)
        delete global_locator;
}

void zmq::locator_t::create (i_thread *calling_thread_,
    unsigned char type_id_, const char *object_,
    i_thread *thread_, i_engine *engine_, scope_t scope_,
    const char *interface_, i_thread *listener_thread_,
    int handler_thread_count_, i_thread **handler_threads_)
{
    assert (type_id_ < type_id_count);
    assert (strlen (object_) < 256);

    //  Enter critical section.
    sync.lock ();

    //  Add the object to the list of known objects.
    object_info_t info = {thread_, engine_};
    objects [type_id_].insert (objects_t::value_type (object_, info));

    //  Add the object to the global locator.
    if (scope_ == scope_global) {

        assert (global_locator);
        assert (strlen (interface_) < 256);
         
        //  Create a listener for the object.
        i_listener *listener = pollable_factory_t::create_listener (
            calling_thread_, listener_thread_, interface_,
            handler_thread_count_, handler_threads_,
            type_id_ == exchange_type_id ? false : true,
            thread_, engine_, object_);
                  
        //  Send to 'create' command.
        unsigned char cmd = create_id;
        global_locator->write (&cmd, 1);
        unsigned char type_id = type_id_;
        global_locator->write (&type_id, 1);         
        unsigned char size = (unsigned char) strlen (object_);
        global_locator->write (&size, 1);
        global_locator->write (object_, size);
        size = (unsigned char) strlen (listener->get_arguments ());
        global_locator->write (&size, 1);
        global_locator->write (listener->get_arguments (), size);

        //  Read the response.
        global_locator->read (&cmd, 1);
        assert (cmd == create_ok_id);
    }

    //  Leave critical section.
    sync.unlock ();
}

bool zmq::locator_t::get (i_thread *calling_thread_, unsigned char type_id_,
    const char *object_, i_thread **thread_, i_engine **engine_,
    i_thread *handler_thread_, const char *local_object_,
    const char *engine_arguments_)
{
    assert (type_id_ < type_id_count);
    assert (strlen (object_) < 256);

    //  Enter critical section.
    sync.lock ();

    //  Find the object.
    objects_t::iterator it = objects [type_id_].find (object_);

    //  If the object is unknown, find it using global locator.
    if (it == objects [type_id_].end ()) {

        //  If we are running without global locator, fail.
        if (!global_locator) {
            sync.unlock ();
            return false;
        }

        //  Send 'get' command.
        unsigned char cmd = get_id;
        global_locator->write (&cmd, 1);
        unsigned char type_id = type_id_;
        global_locator->write (&type_id, 1);
        unsigned char size = (unsigned char) strlen (object_);
        global_locator->write (&size, 1);
        global_locator->write (object_, size);

        //  Read the response.
        global_locator->read (&cmd, 1);
        if (cmd == fail_id) {

            //  Leave critical section.
            sync.unlock ();

            return false;
        }

        assert (cmd == get_ok_id);
        global_locator->read (&size, 1);
        char iface [256];
        global_locator->read (iface, size);
        iface [size] = 0;

        //  Create the proxy engine for the object.
        i_pollable *engine = pollable_factory_t::create_engine (calling_thread_,
            handler_thread_, iface, local_object_, engine_arguments_);

        //  Write it into object repository.
        object_info_t info = {handler_thread_, engine};
        it = objects [type_id_].insert (
            objects_t::value_type (object_, info)).first;
    }

    *thread_ = it->second.thread;
    *engine_ = it->second.engine;

    //  Leave critical section.
    sync.unlock ();

    return true;
}
