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

#ifndef __ZMQ_YSEMAPHORE_HPP_INCLUDED__
#define __ZMQ_YSEMAPHORE_HPP_INCLUDED__

#include <assert.h>

#include <zmq/platform.hpp>
#include <zmq/i_signaler.hpp>
#include <zmq/err.hpp>

#if (defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_OPENVMS)
#include <pthread.h>
#elif defined ZMQ_HAVE_WINDOWS
#include <zmq/windows.hpp>
#else
#include <semaphore.h>
#endif

namespace zmq
{

    //  Simple semaphore. Only single thread may be waiting at any given time.
    //  Also, the semaphore may not be posted before the previous post
    //  was matched by corresponding wait and the waiting thread was
    //  released.
    //
    //  Implementation notes:
    //
    //  - Default implementation uses POSIX semaphore.
    //  - On Linux and OS X platform optimised version is supplied using
    //    mutex instead of semaphore.
    //  - On Windows platform simple semaphore is implemented using
    //    event object.

#if (defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_OPENVMS)

    class ysemaphore_t : public i_signaler
    { 
    public:

        //  Initialise the semaphore.
        inline ysemaphore_t ()
        {
            int rc = pthread_mutex_init (&mutex, NULL);
	    errno_assert (rc == 0);
            rc = pthread_mutex_lock (&mutex);
	    errno_assert (rc == 0);
        }

        //  Destroy the semaphore.
        inline ~ysemaphore_t ()
        {
            int rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
            rc = pthread_mutex_destroy (&mutex);
            errno_assert (rc == 0);
        }

        //  Wait for the semaphore.
        inline void wait ()
        {
             int rc = pthread_mutex_lock (&mutex);
             errno_assert (rc == 0);
        }

        //  Post the semaphore.
        void signal (int signal_);

    private:

        //  Simple semaphore is implemented by mutex, as it is more efficient
        //  on Linux platform.
        pthread_mutex_t mutex;

        //  Disable copying of ysemaphore object.
        ysemaphore_t (const ysemaphore_t&);
        void operator = (const ysemaphore_t&);
    };

#elif defined ZMQ_HAVE_WINDOWS

    class ysemaphore_t : public i_signaler
    { 
    public:

        //  Initialise the semaphore.
        inline ysemaphore_t ()
        {
            ev = CreateEvent (NULL, FALSE, FALSE, NULL);
            win_assert (ev != NULL);
        }

        //  Destroy the semaphore.
        inline ~ysemaphore_t ()
        {
            int rc = CloseHandle (ev);
            win_assert (rc != 0);    
        }

        //  Wait for the semaphore.
        inline void wait ()
        {
            DWORD rc = WaitForSingleObject (ev, INFINITE);
            win_assert (rc != WAIT_FAILED);
        }

        //  Post the semaphore (i_signaler implementation).
        void signal (int signal_);

    private:

        HANDLE ev;

        //  Disable copying of ysemaphore object.
        ysemaphore_t (const ysemaphore_t&);
        void operator = (const ysemaphore_t&);
    };

#else

    class ysemaphore_t : public i_signaler
    { 
    public:

        //  Initialise the semaphore.
        inline ysemaphore_t ()
        {
             int rc = sem_init (&sem, 0, 0);
             errno_assert (rc != -1);
        }

        //  Destroy the semaphore.
        inline ~ysemaphore_t ()
        {
             int rc = sem_destroy (&sem);
             errno_assert (rc != -1);
        }

        //  Wait for the semaphore.
        inline void wait ()
        {
             int rc = sem_wait (&sem);
             errno_assert (rc != -1);
        }

        //  Post the semaphore.
        void signal (int signal_);

    private:

        //  Underlying system semaphore object.
        sem_t sem;

        //  Disable copying of ysemaphore object.
        ysemaphore_t (const ysemaphore_t&);
        void operator = (const ysemaphore_t&);
    };

#endif

}

#endif
