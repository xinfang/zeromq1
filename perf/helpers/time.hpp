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

#ifndef __PERF_TIME_HPP_INCLUDED__
#define __PERF_TIME_HPP_INCLUDED__

#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

namespace perf
{
    //  Time instance in microseconds
    //  For measurement purposes the exact point when the timer started
    //  (e.g. midnight January 1, 1970) is irrelevant. The only requirement
    //  is that all times are measured from the same starting point.
    typedef unsigned long long time_instant_t;

    //  Time interval in microseconds
    typedef unsigned long long time_interval_t;

    //  Get current time
    inline time_instant_t now ()
    {
#if (defined (__PERF_USE_TSC__) && defined (__GNUC__))
       uint32_t low;
       uint32_t high;
       __asm__ volatile ("rdtsc"
           : "=a" (low), "=d" (high));
       return (time_instant_t) high << 32 | low;
#else
        timeval tv;
        int rc = gettimeofday (&tv, NULL);
        assert (rc == 0);
        return ((time_t) tv.tv_sec) * 1000000 + tv.tv_usec;
#endif
    }

}

#endif
