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

#include <assert.h>
#include <string.h>
#ifndef ZMQ_HAVE_WINXP
#include <arpa/inet.h>
#endif

#include "err.hpp"
#include "locator.hpp"
#include "bp_listener.hpp"
#include "bp_engine.hpp"
#ifndef ZMQ_HAVE_WINXP
#include "config.hpp"
#endif

zmq::locator_t::locator_t (const char *hostname_)
{
    if (hostname_) {

        //  If port number is not explicitly specified, use the default one.
        if (!strchr (hostname_, ':')) {
            char buf [256];
            snprintf (buf, 256, "%s:%d", hostname_, (int) default_locator_port);
            hostname_ = buf;
        }

        //  Open connection to global locator.
        global_locator = new tcp_socket_t (hostname_, true);
    }
    else
        global_locator = NULL;

    int rc = pthread_mutex_init (&sync, NULL);
    errno_assert (rc == 0);
}

zmq::locator_t::~locator_t ()
{
    int rc = pthread_mutex_destroy (&sync);
    errno_assert (rc == 0);

    if (global_locator)
        delete global_locator;
}

void zmq::locator_t::create (unsigned char type_id_, const char *object_,
    i_context *context_, i_engine *engine_, scope_t scope_,
    const char *interface_, poll_thread_t *listener_thread_,
    int handler_thread_count_, poll_thread_t **handler_threads_)
{
    assert (type_id_ < type_id_count);
    assert (strlen (object_) < 256);

    //  Enter critical section.
    int rc = pthread_mutex_lock (&sync);
    errno_assert (rc == 0);

    //  Add the object to the list of known objects.
    object_info_t info = {context_, engine_};
    objects [type_id_].insert (objects_t::value_type (object_, info));

    //  Add the object to the global locator.
    if (scope_ == scope_global) {

         assert (global_locator);
         assert (strlen (interface_) < 256);
         
         //  Create a listener for the object.
         bp_listener_t *listener = bp_listener_t::create (listener_thread_,
             interface_, handler_thread_count_, handler_threads_,
             type_id_ == exchange_type_id ? false : true, context_,
             engine_, object_);
                  
         //  Send to 'create' command.
         unsigned char cmd = create_id;
         global_locator->write (&cmd, 1);
         unsigned char type_id = type_id_;
         global_locator->write (&type_id, 1);         
         unsigned char size = strlen (object_);
         global_locator->write (&size, 1);
         global_locator->write (object_, size);
         size = strlen (listener->get_interface ());
         global_locator->write (&size, 1);
         global_locator->write (listener->get_interface (), size);

         //  Read the response.
         global_locator->read (&cmd, 1);
         assert (cmd == create_ok_id);
    }

    //  Leave critical section.
    rc = pthread_mutex_unlock (&sync);
    errno_assert (rc == 0);
}

bool zmq::locator_t::get (unsigned char type_id_, const char *object_,
    i_context **context_, i_engine **engine_, poll_thread_t *thread_,
    const char *local_object_)
{
    assert (type_id_ < type_id_count);
    assert (strlen (object_) < 256);

    //  Enter critical section.
    int rc = pthread_mutex_lock (&sync);
    errno_assert (rc == 0);

    //  Find the object.
    objects_t::iterator it = objects [type_id_].find (object_);

    //  If the object is unknown, find it using global locator.
    if (it == objects [type_id_].end ()) {

         //  If we are running without global locator, fail.
         if (!global_locator) {

             //  Leave critical section.
             rc = pthread_mutex_unlock (&sync);
             errno_assert (rc == 0);

             return false;
         }

         //  Send 'get' command.
         unsigned char cmd = get_id;
         global_locator->write (&cmd, 1);
         unsigned char type_id = type_id_;
         global_locator->write (&type_id, 1);
         unsigned char size = strlen (object_);
         global_locator->write (&size, 1);
         global_locator->write (object_, size);

         //  Read the response.
         global_locator->read (&cmd, 1);
         if (cmd == fail_id) {

             //  Leave critical section.
             rc = pthread_mutex_unlock (&sync);
             errno_assert (rc == 0);

             return false;
         }

         assert (cmd == get_ok_id);
         global_locator->read (&size, 1);
         char interface [256];
         global_locator->read (interface, size);
         interface [size] = 0;

         //  Create the proxy engine for the object.
         bp_engine_t *engine = bp_engine_t::create (thread_,
             interface, bp_out_batch_size, bp_in_batch_size,
             local_object_);

         //  Write it into object repository.
         object_info_t info = {thread_, engine};
         it = objects [type_id_].insert (
             objects_t::value_type (object_, info)).first;
    }

    *context_ = it->second.context;
    *engine_ = it->second.engine;

    //  Leave critical section.
    rc = pthread_mutex_unlock (&sync);
    errno_assert (rc == 0);

    return true;
}
