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

#include "atomic_uint32.hpp"

uint32_t zmq::atomic_uint32_t::izte (uint32_t thenval_, uint32_t elseval_)
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

