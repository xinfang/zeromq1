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

#include "thread.hpp"
#include "err.hpp"

#ifdef ZMQ_HAVE_WINDOWS

zmq::thread_t::thread_t (thread_fn *tfn_, void *arg_) :
    tfn (tfn_),
    arg (arg_)
{
    descriptor = CreateThread (NULL, 0, &zmq::thread_t::thread_routine,
        this, 0, NULL); 
    win_assert (descriptor != NULL);
    
}

zmq::thread_t::~thread_t ()
{
    DWORD rc = WaitForSingleObject (descriptor, INFINITE);
    win_assert (rc != WAIT_FAILED);
}

DWORD WINAPI zmq::thread_t::thread_routine (LPVOID arg_)
{
    thread_t *self = (thread_t*) arg_;
    self->tfn (self->arg);
    return 0;
}

#else

zmq::thread_t::thread_t (thread_fn *tfn_, void *arg_) :
    tfn (tfn_),
    arg (arg_)
{
    int rc = pthread_create (&descriptor, NULL, thread_routine, this);
    errno_assert (rc == 0);
}

zmq::thread_t::~thread_t ()
{
    int rc = pthread_join (descriptor, NULL);
    errno_assert (rc == 0);
}

void *zmq::thread_t::thread_routine (void *arg_)
{
    thread_t *self = (thread_t*) arg_;   
    self->tfn (self->arg);
    return NULL;
}

#endif





