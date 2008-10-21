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

#include <algorithm>
#include <assert.h>

#include "dispatcher.hpp"
#include "err.hpp"


zmq::dispatcher_t::dispatcher_t (int thread_count_) :
    thread_count (thread_count_),
    signalers (thread_count, (i_signaler*) NULL),
    used (thread_count, false)
{
    //  Alocate NxN matrix of dispatching pipes.
    pipes = new command_pipe_t [thread_count * thread_count];
    assert (pipes);

#ifdef ZMQ_HAVE_WINDOWS

    //  Intialise Windows sockets. Note that WSAStartup can be called multiple
    //  times given that WSACleanup will be called for each WSAStartup.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    errno_assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 || HIBYTE (wsa_data.wVersion) == 2);
#endif  
}

zmq::dispatcher_t::~dispatcher_t ()
{
    //  Deallocate the pipe matrix.
    delete [] pipes;

#ifdef ZMQ_HAVE_WINDOWS

    //  Uninitialise Windows sockets.
    WSACleanup ();
#endif
}

int zmq::dispatcher_t::allocate_thread_id (i_signaler *signaler_)
{
    //  Lock the mutex.
    sync.lock ();

    //  Find the first free thread ID.
    std::vector <bool>::iterator it = std::find (used.begin (),
        used.end (), false);

    //  No more thread IDs are available!
    assert (it != used.end ());

    //  Mark the thread ID as used.
    *it = true;
    int thread_id = it - used.begin ();

    //  Unlock the mutex.
    sync.unlock ();

    //  Set the signaler.
    signalers [thread_id] = signaler_;

    return thread_id;
}

void zmq::dispatcher_t::deallocate_thread_id (int thread_id_)
{
    //  Lock the mutex.
    sync.lock ();

    //  Free the specified thread ID.
    assert (used [thread_id_] == true);
    used [thread_id_] = false;

    //  Unregister signaler.
    signalers [thread_id_] = NULL;

    //  Unlock the mutex.
    sync.unlock ();
}

