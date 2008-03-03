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

#ifndef __ZMQ_ATOMIC_UINT32_HPP_INCLUDED__
#define __ZMQ_ATOMIC_UINT32_HPP_INCLUDED__

#include <pthread.h>

#include "err.hpp"
#include "stdint.hpp"

namespace zmq
{

    //  This class encapuslates several atomic operations on unsigned 32-bit
    //  integer. Selection of the provided instructions is driven specifically
    //  by the needs of ypollset implementation.
    //
    //  Note that i386 and x86_64 implementations are dependend on processors'
    //  full memory barrier associated with atomic operations (lock prefix).
    //  On different platforms memory fencing may be required to be implemented
    //  explicitly.
    class atomic_uint32_t
    {
    public:

        inline atomic_uint32_t ()
        {
            value = 0;
#if (!defined (__GNUC__) || (!defined (__i386__) && !defined (__x86_64__)))
            int rc = pthread_mutex_init (&mutex, NULL);
            errno_assert (rc == 0);
#endif
        }

        inline ~atomic_uint32_t ()
        {
#if (!defined (__GNUC__) || (!defined (__i386__) && !defined (__x86_64__)))
            int rc = pthread_mutex_destroy (&mutex);
            errno_assert (rc == 0);
#endif
        }

        //  Bit-test-set-and-reset. Sets one bit of the value and resets
        //  another one. Returns the original value of the reseted bit.
        //
        //  There's no need for the operation to be fully atomic. The only
        //  requirement is that setting of the bit is performed first and
        //  resetting afterwards. Getting the original value of the reseted
        //  bit and actual reset, however, have to be done atomically.
        inline bool btsr (int set_index_, int reset_index_)
        {
#if ((defined (__i386__) || defined (__x86_64__)) && defined (__GNUC__))
            uint32_t oldval;
            __asm__ volatile (
                "lock; btsl %1, (%3)\n\t"  //  Does bts have to be atomic?
                "lock; btrl %2, (%3)\n\t"
                "setc %%al\n\t"
                "movzb %%al, %0\n\t"
                : "=r" (oldval)
                : "r" (set_index_), "r" (reset_index_), "r" (&value)
                : "memory", "cc", "%eax");
            return (bool) oldval;
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            uint32_t oldval = value;
            value = (oldval | (uint32_t (1) << set_index_)) &
                ~(uint32_t (1) << reset_index_);
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
            return (bool) (oldval & (uint32_t (1) << reset_index_));
#endif
        }

        //  Sets value to newval. Returns the original value.
        inline uint32_t xchg (uint32_t newval_)
        {
            uint32_t oldval;
#if ((defined (__i386__) || defined (__x86_64__)) && defined (__GNUC__))
            oldval = newval_;
            __asm__ volatile (
                "lock; xchgl %0, %1"
                : "=r" (oldval)
                : "m" (value), "0"(oldval)
                : "memory");
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            oldval = value;
            value = newval_;
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
#endif
            return oldval;
        }

        //  izte is "if-zero-then-else" atomic operation - if the value is zero
        //  it substitutes it by 'thenval' else it rewrites it by 'elseval'.
        //  Original value of the integer is returned from this function.
        //
        //  As such atomic operation doesn't exist on i386 (and x86_64)
        //  platform, following assumption is made allowing to implement
        //  the operation as a non-atomic sequence of two atomic operations:
        //  "While izte is being called from one thread no other thread is
        //  allowed to perform any operation that would result in clearing
        //  bits of the value (btr, xchg, izte)."
        //  If the code using atomic_uint32 doesn't adhere to this assumption
        //  the behaviour of izte is undefined.
        inline uint32_t izte (uint32_t thenval_, uint32_t elseval_)
        {
            uint32_t oldval;
#if ((defined (__i386__) || defined (__x86_64__)) && defined (__GNUC__))
            __asm__ volatile (
                "lock; cmpxchgl %1, %3\n\t"
                "jz 1f\n\t"
                "mov %2, %%eax\n\t"
                "lock; xchgl %%eax, %3\n\t"
                "1:\n\t"
                : "=&a" (oldval)
                : "r" (thenval_), "r" (elseval_), "m" (value), "0" (0)
                : "memory", "cc");
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            oldval = value;
            value = oldval ? elseval_ : thenval_;
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
#endif
            return oldval;
        }

    protected:

        volatile uint32_t value;
#if (!defined (__GNUC__) || (!defined (__i386__) && !defined (__x86_64__)))
        pthread_mutex_t mutex;
#endif
    };

}

#endif
