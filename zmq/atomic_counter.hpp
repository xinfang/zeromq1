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

#ifndef __ZMQ_ATOMIC_COUNTER_HPP_INCLUDED__
#define __ZMQ_ATOMIC_COUNTER_HPP_INCLUDED__

#include <pthread.h>

#include "err.hpp"
#include "stdint.hpp"

namespace zmq
{

    class atomic_counter_t
    {
    public:

        typedef uint32_t integer_t;

        inline atomic_counter_t (integer_t value_ = 0) :
            value (value_)
        {
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) || (!defined (__i386__)\
    && !defined (__x86_64__)))
            int rc = pthread_mutex_init (&mutex, NULL);
            errno_assert (rc == 0);
#endif
        }

        inline ~atomic_counter_t ()
        {
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) || (!defined (__i386__)\
    && !defined (__x86_64__)))
            int rc = pthread_mutex_destroy (&mutex);
            errno_assert (rc == 0);
#endif
        }

        //  Set counter value (not thread-safe)
        inline void unsafe_set (integer_t value_)
        {
            value = value_;
        }

        //  Non-atomic increment
        inline void unsafe_inc ()
        {
            value ++;
        }

        //  Atomic increment.
        inline void safe_inc ()
        {
#if (!defined (ZMQ_FORCE_MUTEXES) && (defined (__i386__) ||\
    defined (__x86_64__)) && defined (__GNUC__))
            integer_t increment = 1;
            __asm__ volatile ("lock; xaddl %0,%1"
                : "=r" (increment), "=m" ((volatile integer_t) value)
                : "m" ((volatile integer_t) value)
                : "memory", "cc");
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            ((volatile integer_t) value) ++;
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
#endif
        }

        //  Non-atomic decrement. Returns false if the counter drops to zero.
        inline bool unsafe_dec ()
        {
            return -- value;
        }

        //  Atomic decrement. Returns false if the counter drops to zero.
        inline bool safe_dec ()
        {
#if (!defined (ZMQ_FORCE_MUTEXES) && (defined (__i386__) ||\
    defined (__x86_64__)) && defined (__GNUC__))
            integer_t oldval = -1;
            __asm__ volatile ("lock; xaddl %0,%1"
                : "=r" (oldval), "=m" ((volatile integer_t) value)
                : "0" (oldval), "m" ((volatile integer_t) value)
                : "memory", "cc");
            return oldval > 1;
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            bool result = ((volatile integer_t) value) --;
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
            return result;
#endif
        }

    protected:

        integer_t value;
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) ||\
    (!defined (__i386__) && !defined (__x86_64__)))
        pthread_mutex_t mutex;
#endif
    };

}

#endif
