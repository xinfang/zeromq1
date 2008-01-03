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

#ifndef __ZMQ_ATOMIC_PTR_INCLUDED__
#define __ZMQ_ATOMIC_PTR_INCLUDED__

#include <pthread.h>

#include "err.h"

//  128 bytes is the max cache line size we've encountered so far.
//  This value may be tuned for specific microarchitecture to save few
//  bytes of memory, however, given that 0MQ uses just a limited number
//  of atomic pointers, there's no much point in doing so.
#define ZMQ_MAX_CACHE_LINE_SIZE 128

namespace zmq
{

    //  This class stores a single pointer and allows you to perform
    //  atomic operations on the pointer.
    //
    //  Note that i386 and x86_64 implementations are dependend on processors'
    //  full memory barrier associated with atomic operations (lock prefix).
    //  On different platforms memory fencing may be required to be implemented
    //  explicitly.
    template <typename T> class atomic_ptr
    {
    public:

        //  Initialise atomic pointer
        atomic_ptr ()
        {
            ptr = NULL;
#if (!defined (__GNUC__) || (!defined (__i386__) && !defined (__x86_64__)))
            int rc = pthread_mutex_init (&mutex, NULL);
            errno_assert (rc == 0);
#endif
        }

        //  Destroy atomic pointer
        ~atomic_ptr ()
        {
#if (!defined (__GNUC__) || (!defined (__i386__) && !defined (__x86_64__)))
            int rc = pthread_mutex_destroy (&mutex);
            errno_assert (rc == 0);
#endif
        }

        //  Set value of atomic pointer in a non-threadsafe way
        //  Use this function only when you are sure that at most one
        //  thread is accessing the pointer at the moment.
        void set (T *ptr)
        {
            this->ptr = ptr;
        }

        //  Perform atomic 'compare and swap' operation on the pointer.
        //  The pointer is compared to 'cmp' argument and if they are
        //  equal, its value is set to 'val'. Old value of the pointer
        //  is returned in any case.
        T *cas (T *cmp, T *val)
        {
#if (defined (__i386__) && defined (__GNUC__))
            T *old;
            __asm__ volatile ("lock; cmpxchgl %1, %2"             
                : "=a" (old)               
                : "r" (val), "m" (ptr), "0" (cmp) 
                : "memory", "cc");
            return old;
#elif (defined (__x86_64__) && defined (__GNUC__))
            T *old;
            __asm__ volatile ("lock; cmpxchgq %1, %2"             
                : "=a" (old)               
                : "r" (val), "m" (ptr), "0" (cmp) 
                : "memory", "cc");
            return old;
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            T *old = (T*) ptr;
            if (ptr == cmp)
                ptr = val;
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
            return old;
#endif
        }
    protected:
        
#ifdef __GNUC__
        //  On GNU C platform, pointer is aligned and padded in the way
        //  that is fits into single cache line.
        volatile T *ptr __attribute__ ((aligned (ZMQ_MAX_CACHE_LINE_SIZE)));
        char padding [ZMQ_MAX_CACHE_LINE_SIZE - sizeof (T*)];
#else
        volatile T *ptr;
#endif

#if (!defined (__GNUC__) || (!defined (__i386__) && !defined (__x86_64__)))
        pthread_mutex_t mutex;
#endif
    };

}

#endif
