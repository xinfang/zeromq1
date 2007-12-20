/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __ZMQ_YSEMAPHORE_HPP_INCLUDED__
#define __ZMQ_YSEMAPHORE_HPP_INCLUDED__

#include <semaphore.h>

#include "err.hpp"

namespace zmq
{

    //  Simple semaphore. Only single thread may be waiting at any given time.
    //  Also, the semaphore may not be posted before the previous post
    //  was matched by corresponding wait and the waiting thread was
    //  released.
    class ysemaphore_t
    { 
    public:

        //  Initialise the semaphore
        inline ysemaphore_t ()
        {
            int rc = pthread_mutex_init (&mutex, NULL);
	    errno_assert (rc == 0);
            rc = pthread_mutex_lock (&mutex);
	    errno_assert (rc == 0);
        }

        //  Destroy the semaphore
        inline ~ysemaphore_t ()
        {
            int rc = pthread_mutex_unlock (&mutex);
	    errno_assert (rc == 0);
            rc = pthread_mutex_destroy (&mutex);
	    errno_assert (rc == 0);
        }

        //  Wait for the semaphore
        inline void wait ()
        {
             int rc = pthread_mutex_lock (&mutex);
             errno_assert (rc == 0);
        }

        //  Post the semaphore
        inline void post ()
        {
            int rc = pthread_mutex_unlock (&mutex);
	    errno_assert (rc == 0);
        }
    protected:

        //  Simple semaphore is implemented by mutex, as it is more efficient
        //  on Linux platform.
        pthread_mutex_t mutex;
    };

}

#endif
