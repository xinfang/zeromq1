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

#include <algorithm>

#include <zmq/platform.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/engine_factory.hpp>
#include <zmq/err.hpp>


zmq::dispatcher_t::dispatcher_t (int thread_count_) :
    thread_count (thread_count_),
    signalers (thread_count, (i_signaler*) NULL),
    used (thread_count, false)
{
    //  Alocate NxN matrix of dispatching pipes.
    pipes = new command_pipe_t [thread_count * thread_count];
    zmq_assert (pipes);

#ifdef ZMQ_HAVE_WINDOWS

    //  Intialise Windows sockets. Note that WSAStartup can be called multiple
    //  times given that WSACleanup will be called for each WSAStartup.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    errno_assert (rc == 0);
    zmq_assert (LOBYTE (wsa_data.wVersion) == 2 ||
        HIBYTE (wsa_data.wVersion) == 2);
#endif  
}

zmq::dispatcher_t::~dispatcher_t ()
{
    //  Initiate termination of worker threads.
    for (std::vector <i_thread*>::iterator it = threads.begin ();
          it != threads.end (); it ++)
        (*it)->stop ();

    //  Wait while worker thread termination is completed.
    for (std::vector <i_thread*>::iterator it = threads.begin ();
          it != threads.end (); it ++)
        (*it)->destroy ();

    //  Deallocate the pipe matrix.
    delete [] pipes;

#ifdef ZMQ_HAVE_WINDOWS

    //  Uninitialise Windows sockets.
    //  int rc = WSACleanup ();
    //  wsa_assert (rc != SOCKET_ERROR);
#endif
}

int zmq::dispatcher_t::allocate_thread_id (i_thread *thread_,
    i_signaler *signaler_)
{
    //  Lock the mutex.
    sync.lock ();

    //  Find the first free thread ID.
    std::vector <bool>::iterator it = std::find (used.begin (),
        used.end (), false);

    //  No more thread IDs are available!
    zmq_assert (it != used.end ());

    //  Mark the thread ID as used.
    *it = true;
    int thread_id = it - used.begin ();

    //  Unlock the mutex.
    sync.unlock ();

    //  Set the signaler.
    signalers [thread_id] = signaler_;

    //  Store the pointer to the thread to be used during shutdown.
    threads.push_back (thread_);

    return thread_id;
}

void zmq::dispatcher_t::create (i_locator *locator_, i_thread *calling_thread_,
    bool sender_, const char *object_, i_thread *thread_,
    i_engine *engine_, scope_t scope_, const char *location_,
    i_thread *listener_thread_, int handler_thread_count_,
    i_thread **handler_threads_)
{
    zmq_assert (strlen (object_) < 256);

    //  Enter critical section.
    sync.lock ();

    //  Add the object to the list of known objects.
    object_info_t info = {thread_, engine_};
    objects.insert (objects_t::value_type (object_, info));

    //  Add the object to the global locator.
    if (scope_ == scope_global) {

        //  If location of the global object is not specified, retrieve it
        //  from the directory service.
        char buff [256];
        if (!location_ || strlen (location_) == 0) {
            locator_->resolve_endpoint (object_, buff, sizeof (buff));
            location_ = buff;
        }

        //  Create a listener for the object.
        i_engine *listener = engine_factory_t::create_listener (object_,
            sender_, location_, calling_thread_, listener_thread_,
            handler_thread_count_, handler_threads_, thread_, engine_);

        //  Regiter the object with the locator.
        locator_->register_endpoint (object_, listener->get_arguments ());
    }

    //  Leave critical section.
    sync.unlock ();
}

bool zmq::dispatcher_t::get (i_locator *locator_, i_thread *calling_thread_,
    bool sender_, const char *object_, i_thread **thread_, i_engine **engine_,
    i_thread *handler_thread_, const char *local_object_,
    const char *engine_arguments_)
{
    zmq_assert (strlen (object_) < 256);

    //  Enter critical section.
    sync.lock ();

    //  Find the object.
    objects_t::iterator it = objects.find (object_);

    //  If the object is unknown, find it using global locator.
    if (it == objects.end ()) {

        //  Get the location of the object from the locator.
        char location [256];
        locator_->resolve_endpoint (object_, location, sizeof (location));

        //  Create the proxy engine for the object.
        i_engine *engine = engine_factory_t::create (local_object_,
            sender_, location, engine_arguments_, calling_thread_,
            handler_thread_);

        //  Write it into object repository.
        object_info_t info = {handler_thread_, engine};
        it = objects.insert (objects_t::value_type (object_, info)).first;
    }

    *thread_ = it->second.thread;
    *engine_ = it->second.engine;

    //  Leave critical section.
    sync.unlock ();

    return true;
}

zmq::error_handler_t * volatile zmq::eh = NULL;

zmq::error_handler_t *zmq::get_error_handler ()
{
    return eh;
}
void zmq::set_error_handler (zmq::error_handler_t *eh_)
{
    eh = eh_;
}




