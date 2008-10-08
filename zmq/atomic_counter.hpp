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


#ifndef __ZMQ_ATOMIC_COUNTER_HPP_INCLUDED__
#define __ZMQ_ATOMIC_COUNTER_HPP_INCLUDED__

#include "err.hpp"
#include "stdint.hpp"
#include "mutex.hpp"
#include "platform.hpp"
#ifdef ZMQ_HAVE_WINDOWS
#include <windows.h>
#endif

namespace zmq
{

    //  This class represents an integer that can be incremented/decremented
    //  in atomic fashion.

    class atomic_counter_t
    {
    public:

        typedef uint32_t integer_t;

        inline atomic_counter_t (integer_t value_ = 0) :
            value (value_)
        {
        }

        inline ~atomic_counter_t ()
        {
        }

        //  Set counter value (not thread-safe).
        inline void set (integer_t value_)
        {
            value = value_;
        }

        //  Atomic addition. Returns false if counter was zero
        //  before the operation.
        inline bool add (integer_t increment)
        {
#if !defined (ZMQ_FORCE_MUTEXES) && defined (ZMQ_HAVE_WINDOWS)
            integer_t old = InterlockedExchangeAdd ((LONG*) &value,
                increment);
			return old != 0;
#elif (!defined (ZMQ_FORCE_MUTEXES) && (defined (__i386__) ||\
    defined (__x86_64__)) && defined (__GNUC__))
            volatile integer_t *val = &value;
            __asm__ volatile ("lock; xaddl %0,%1"
                : "=r" (increment), "=m" (*val)
                : "0" (increment), "m" (*val)
                : "cc");
            return increment;
#elif (0 && !defined (ZMQ_FORCE_MUTEXES) && defined (__sparc__) &&\
    defined (__GNUC__))
            volatile integer_t *val = &value;
            integer_t tmp;
            integer_t result;
            __asm__ volatile(
                "ld [%4], %1\n\t"
                "1:\n\t"
                "add %1, %0, %2\n\t"
                "cas [%4], %1, %2\n\t"
                "cmp %1, %2\n\t"
                "bne,a,pn %%icc, 1b\n\t"
                "mov %2, %1\n\t"
                : "+r" (increment), "=&r" (tmp), "=&r" (result), "+m" (*val)
                : "r" (val)
                : "cc");
            return result; 
#else
            sync.lock ();
			bool result = value ? true : false;
            value += increment;
            sync.unlock ();
            return result;
#endif
        }

        //  Atomic subtraction. Returns false if the counter drops to zero.
        inline bool sub (integer_t decrement)
        {
#if !defined (ZMQ_FORCE_MUTEXES) && defined (ZMQ_HAVE_WINDOWS)
            integer_t old = InterlockedExchangeAdd ((LONG*) &value,
                -decrement);
			return old - decrement != 0;
#elif (!defined (ZMQ_FORCE_MUTEXES) && (defined (__i386__) ||\
    defined (__x86_64__)) && defined (__GNUC__))
            integer_t oldval = -decrement;
            volatile integer_t *val = &value;
            __asm__ volatile ("lock; xaddl %0,%1"
                : "=r" (oldval), "=m" (*val)
                : "0" (oldval), "m" (*val)
                : "cc");
            return oldval != decrement;
#elif (0 && !defined (ZMQ_FORCE_MUTEXES) && defined (__sparc__) &&\
    defined (__GNUC__))
            volatile integer_t *val = &value;
            integer_t tmp;
            integer_t result;
            __asm__ volatile(
                "ld [%4], %1\n\t"
                "1:\n\t"
                "add %1, %0, %2\n\t"
                "cas [%4], %1, %2\n\t"
                "cmp %1, %2\n\t"
                "bne,a,pn %%icc, 1b\n\t"
                "mov %2, %1\n\t"
                : "+r" (-decrement), "=&r" (tmp), "=&r" (result), "+m" (*val)
                : "r" (val)
                : "cc");
            return result <= decrement;
#else
            sync.lock ();
            value -= decrement;
            bool result = value ? true : false;
            sync.unlock ();
            return result;
#endif
        }

    private:

        volatile integer_t value;
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) ||\
    (!defined (__i386__) && !defined (__x86_64__) /*&& !defined (__sparc__)*/))
        mutex_t sync;
#endif

        atomic_counter_t (const atomic_counter_t&);
        void operator = (const atomic_counter_t&);
    };

}

#endif
