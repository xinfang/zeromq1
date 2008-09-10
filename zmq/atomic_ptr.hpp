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

#ifndef __ZMQ_ATOMIC_PTR_HPP_INCLUDED__
#define __ZMQ_ATOMIC_PTR_HPP_INCLUDED__

#include <pthread.h>

#include "err.hpp"

namespace zmq
{

    //  This class encapsulates several atomic operations on pointers.

    template <typename T> class atomic_ptr_t
    {
    public:

        //  Initialise atomic pointer
        atomic_ptr_t ()
        {
            ptr = NULL;
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) || (!defined (__i386__)\
    && !defined (__x86_64__)))
            int rc = pthread_mutex_init (&mutex, NULL);
            errno_assert (rc == 0);
#endif
        }

        //  Destroy atomic pointer
        ~atomic_ptr_t ()
        {
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) || (!defined (__i386__)\
    && !defined (__x86_64__)))
            int rc = pthread_mutex_destroy (&mutex);
            errno_assert (rc == 0);
#endif
        }

        //  Set value of atomic pointer in a non-threadsafe way
        //  Use this function only when you are sure that at most one
        //  thread is accessing the pointer at the moment.
        void set (T *ptr_)
        {
            this->ptr = ptr_;
        }

        //  Perform atomic 'exchange pointers' operation. Pointer is set
        //  to the 'val' value. Old value is returned.
        T *xchg (T *val_)
        {
#if (!defined (ZMQ_FORCE_MUTEXES) && defined (__i386__) &&\
    defined (__GNUC__))
            T *old = val_;
            __asm__ volatile ("lock; xchgl %0, %1"
                : "=r" (old)
                : "m" (ptr), "0" (old)
                : "memory");
            return old;
#elif (!defined (ZMQ_FORCE_MUTEXES) && defined (__x86_64__) &&\
    defined (__GNUC__))
            T *old = val_;
            __asm__ volatile ("lock; xchgq %0, %2"
                : "=r" (old), "+m" (*ptr)
                : "m" (ptr), "0" (old)
                : "memory");
            return old;
#elif (!defined (ZMQ_FORCE_MUTEXES) && defined (__sparc__) &&\
    defined (__GNUC__))
            T* newptr = val_;
            volatile T** ptrin = &ptr;
            T* tmp;
            T* prev;
            __asm__ __volatile__(
                "ld [%4], %1\n\t" // tmp = [ptr]
                "1:\n\t"          // 
                "mov %0, %2\n\t"  // prev = newptr
                "cas [%4], %1, %2\n\t" // [ptr], tmp, prev
                "cmp %1, %2\n\t"       // if tmp != prev  
                "bne,a,pn %%icc, 1b\n\t" // 
                "mov %2, %1\n\t" // tmp = prev
                : "+r" (newptr), "=&r" (tmp), "=&r" (prev), "+m" (*ptrin)
                : "r" (ptrin) 
                : "cc");
            return prev; 
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            T *old = (T*) ptr;
            ptr = val_;
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
            return old;
#endif
        }

        //  Perform atomic 'compare and swap' operation on the pointer.
        //  The pointer is compared to 'cmp' argument and if they are
        //  equal, its value is set to 'val'. Old value of the pointer
        //  is returned.
        T *cas (T *cmp_, T *val_)
        {
#if (!defined (ZMQ_FORCE_MUTEXES) && defined (__i386__) &&\
    defined (__GNUC__))
            T *old;
            __asm__ volatile ("lock; cmpxchgl %2, %3"             
                : "=a" (old), "+m" (*ptr)               
                : "r" (val_), "m" (ptr), "0" (cmp_) 
                : "cc");
            return old;
#elif (!defined (ZMQ_FORCE_MUTEXES) && defined (__x86_64__) &&\
    defined (__GNUC__))
            T *old;
            __asm__ __volatile__ ("lock; cmpxchgq %2, %3"             
                : "=a" (old), "+m" (*ptr)               
                : "r" (val_), "m" (ptr), "0" (cmp_) 
                : "cc");
            return old;
#elif (!defined (ZMQ_FORCE_MUTEXES) && defined (__sparc__) &&\
    defined (__GNUC__))
            volatile T** ptrin = &ptr;
            volatile T* prev = ptr;
            __asm__ __volatile__(
                "cas [%3], %1, %2\n\t" // [ptr], cmp, tmp2 
                : "+m" (*ptrin)
                : "r" (cmp_), "r" (val_), "r" (ptrin)
                : "cc");
            return (T*)prev; 
#else
            int rc = pthread_mutex_lock (&mutex);
            errno_assert (rc == 0);
            T *old = (T*) ptr;
            if (ptr == cmp_)
                ptr = val_;
            rc = pthread_mutex_unlock (&mutex);
            errno_assert (rc == 0);
            return old;
#endif
        }

    private:
        
        volatile T *ptr;
#if (defined (ZMQ_FORCE_MUTEXES) || !defined (__GNUC__) || (!defined (__i386__)\
    && !defined (__x86_64__)))
        pthread_mutex_t mutex;
#endif

        atomic_ptr_t (const atomic_ptr_t&);
        void operator = (const atomic_ptr_t&);
    };

}

#endif
