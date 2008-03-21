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

#include <algorithm>
#include <assert.h>

#include "dispatcher.hpp"
#include "err.hpp"


zmq::dispatcher_t::dispatcher_t (int engine_count_) :
    engine_count (engine_count_),
    used (engine_count_)
{
    //  Alocate N * N matrix of dispatching pipes
    cells = new cell_t [engine_count * engine_count];
    assert (cells);
    for (int cell_nbr = 0; cell_nbr != engine_count * engine_count; cell_nbr ++)
        cells [cell_nbr].signaler = NULL;

    //  Initialise the mutex
    int rc = pthread_mutex_init (&mutex, NULL);
    errno_assert (rc == 0);

    //  Mark all the engine IDs as unused
    std::fill (used.begin (), used.end (), false);
}

zmq::dispatcher_t::~dispatcher_t ()
{
    //  Uninitialise the mutex
    int rc = pthread_mutex_destroy (&mutex);
    errno_assert (rc == 0);

    //  Deallocate the pipe matrix
    delete [] cells;
}

void zmq::dispatcher_t::set_signaler (int engine_id_, i_signaler *signaler_)
{
    for (int engine_nbr = 0; engine_nbr != engine_count; engine_nbr ++)
        cells [engine_nbr * engine_count + engine_id_].signaler = signaler_; 
}

int zmq::dispatcher_t::allocate_engine_id ()
{
    //  Lock the mutex
    int rc = pthread_mutex_lock (&mutex);
    errno_assert (rc == 0);

    //  Find the first free engine ID
    std::vector <bool>::iterator it = std::find (used.begin (),
        used.end (), false);
    assert (it != used.end ());  //  No more IDs are available!
        *it = true;

    //  Unlock the mutex
    rc = pthread_mutex_unlock (&mutex);
    errno_assert (rc == 0);

    return it - used.begin ();
}

void zmq::dispatcher_t::deallocate_engine_id (int engine_id_)
{
    //  Lock the mutex
    int rc = pthread_mutex_lock (&mutex);
    errno_assert (rc == 0);

    //  Free the specified engine ID
    assert (used [engine_id_] == true);
    used [engine_id_] = false;

    //  Unlock the mutex
    rc = pthread_mutex_unlock (&mutex);
    errno_assert (rc == 0);
}

