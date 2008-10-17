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


#ifndef __ZMQ_ATOMIC_BITMAP_HPP_INCLUDED__
#define __ZMQ_ATOMIC_BITMAP_HPP_INCLUDED__

#include "err.hpp"
#include "stdint.hpp"
#include "mutex.hpp"

namespace zmq
{

    //  This class encapuslates several bitwise atomic operations on unsigned
    //  integer. Selection of operations is driven specifically by the needs
    //  of ypollset implementation.

    class atomic_bitmap_t
    {
    public:

#if defined (__x86_64__)
        typedef uint64_t integer_t;
#else
        typedef uint32_t integer_t;
#endif

        inline atomic_bitmap_t (integer_t value_ = 0) :
            value (value_)
        {
        }

        inline ~atomic_bitmap_t ()
        {
        }

        //  Bit-test-set-and-reset. Sets one bit of the value and resets
        //  another one. Returns the original value of the reset bit.
        //
        //  There's no need for the operation to be fully atomic. The only
        //  requirement is that setting of the bit is performed first and
        //  resetting afterwards. Getting the original value of the reset
        //  bit and actual reset, however, have to be done atomically.
        inline bool btsr (int set_index_, int reset_index_)
        {
#if (!defined (ZMQ_FORCE_MUTEXES) && (defined (__i386__) ||\
    defined (__x86_64__)) && defined (__GNUC__))
            uint32_t oldval;
            __asm__ volatile (
                "lock; btsl %2, (%4)\n\t"  //  Does bts have to be atomic?
                "lock; btrl %3, (%4)\n\t"
                "setc %%al\n\t"
                "movzb %%al, %0\n\t"
                : "=r" (oldval), "+m" (value)
                : "r" (set_index_), "r" (reset_index_), "r" (&value)
                : "memory", "cc", "%eax");
            return (bool) oldval;
#elif (0 && !defined (ZMQ_FORCE_MUTEXES) && defined (__sparc__) &&\
    defined (__GNUC__))
            volatile integer_t* valptr = &value;
            integer_t set_val = integer_t(1) << set_index_;
            integer_t reset_val = ~(integer_t(1) << reset_index_);
            integer_t tmp;
            integer_t oldval;
            __asm__ volatile(
                "ld       [%5], %2       \n\t" 
                "1:                      \n\t" 
                "or       %2, %0, %3     \n\t" 
                "and      %3, %1, %3     \n\t"
                "cas      [%5], %2, %3   \n\t"
                "cmp      %2, %3         \n\t"
                "bne,a,pn %%icc, 1b      \n\t"
                "mov      %3, %2         \n\t"
                : "+r" (set_val), "+r" (reset_val), "=&r" (tmp), "=&r" (oldval), "+m" (*valptr)
                : "r" (valptr)
                : "cc");
            return oldval; 
#else
            sync.lock ();
            integer_t oldval = value;
            value = (oldval | (integer_t (1) << set_index_)) &
                ~(integer_t (1) << reset_index_);
            sync.unlock ();
            return (oldval & (integer_t (1) << reset_index_)) ? true : false;
#endif
        }

        //  Sets value to newval. Returns the original value.
        inline integer_t xchg (integer_t newval_)
        {
            integer_t oldval;
#if (!defined (ZMQ_FORCE_MUTEXES) && defined (__i386__) && defined (__GNUC__))
            oldval = newval_;
            __asm__ volatile (
                "lock; xchgl %0, %1"
                : "=r" (oldval)
                : "m" (value), "0" (oldval)
                : "memory");
#elif (!defined (ZMQ_FORCE_MUTEXES) && defined (__x86_64__) &&\
    defined (__GNUC__))
            oldval = newval_;
            __asm__ volatile (
                "lock; xchgq %0, %1"
                : "=r" (oldval)
                : "m" (value), "0" (oldval)
                : "memory");
#elif (0 && !defined (ZMQ_FORCE_MUTEXES) && defined (__sparc__) &&\
    defined (__GNUC__))
            oldval = value;
            volatile integer_t* ptrin = &value;
            integer_t tmp;
            integer_t prev;
            __asm__ __volatile__(
                "ld [%4], %1\n\t" // tmp = [ptr]
                "1:\n\t"          // 
                "mov %0, %2\n\t"  // prev = newptr
                "cas [%4], %1, %2\n\t" // [ptr], tmp, prev
                "cmp %1, %2\n\t"       // if tmp != prev  
                "bne,a,pn %%icc, 1b\n\t" // 
                "mov %2, %1\n\t" // tmp = prev
                : "+r" (newval_), "=&r" (tmp), "=&r" (prev), "+m" (*ptrin)
                : "r" (ptrin) /* input */
                : "cc");
            return prev; 
#else
            sync.lock ();
            oldval = value;
            value = newval_;
            sync.unlock ();
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
        //  If the code using atomic_bitmap doesn't adhere to this assumption
        //  the behaviour of izte is undefined.
        inline integer_t izte (integer_t thenval_, integer_t elseval_)
        {
            integer_t oldval;
#if (!defined (ZMQ_FORCE_MUTEXES) && defined (__i386__) && defined (__GNUC__))
            __asm__ volatile (
                "lock; cmpxchgl %1, %3\n\t"
                "jz 1f\n\t"
                "mov %2, %%eax\n\t"
                "lock; xchgl %%eax, %3\n\t"
                "1:\n\t"
                : "=&a" (oldval)
                : "r" (thenval_), "r" (elseval_), "m" (value), "0" (0)
                : "memory", "cc");
#elif (!defined (ZMQ_FORCE_MUTEXES) && defined (__x86_64__) &&\
    defined (__GNUC__))
            __asm__ volatile (
                "lock; cmpxchgq %1, %3\n\t"
                "jz 1f\n\t"
                "mov %2, %%rax\n\t"
                "lock; xchgq %%rax, %3\n\t"
                "1:\n\t"
                : "=&a" (oldval)
                : "r" (thenval_), "r" (elseval_), "m" (value), "0" (0)
                : "memory", "cc");
#elif (0 && !defined (ZMQ_FORCE_MUTEXES) && defined (__sparc__) &&\
    defined (__GNUC__))
            oldval = value;
            volatile integer_t* ptrin = &value;
            integer_t tmp;
            integer_t prev;
            __asm__ __volatile__(
                "ld      [%3], %0       \n\t" 
                "mov     0,    %1       \n\t"           
                "cas     [%3], %1, %4   \n\t" 
                "cmp     %0,   %1       \n\t" 
                "be,a,pn %%icc,1f       \n\t" 
                "ld      [%3], %0       \n\t" 
                "cas     [%3], %0, %5   \n\t" 
                "1:                     \n\t"           
                : "=&r" (tmp), "=&r" (prev), "+m" (*ptrin)
                : "r" (ptrin), "r" (thenval_), "r" (elseval_)
                : "cc");
            return prev; 
#else
            sync.lock ();
            oldval = value;
            value = oldval ? elseval_ : thenval_;
            sync.unlock ();
#endif
            return oldval;
        }

    private:

        volatile integer_t value;
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) ||\
    (!defined (__i386__) && !defined (__x86_64__)))
        mutex_t sync;
#endif

        atomic_bitmap_t (const atomic_bitmap_t&);
        void operator = (const atomic_bitmap_t&);
    };

}

#endif
