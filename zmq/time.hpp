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

#ifndef __ZMQ_TIME_HPP_INCLUDED__
#define __ZMQ_TIME_HPP_INCLUDED__

#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

namespace zmq
{
    //  Time instance in nanoseconds.
    //  For measurement purposes the exact point when the timer started
    //  (e.g. midnight January 1, 1970) is irrelevant. The only requirement
    //  is that all times are measured from the same starting point.
    typedef uint64_t time_instant_t;

    //  Time interval in nanoseconds.
    typedef uint64_t time_interval_t;

#if (!defined (PERF_FORCE_GETTIMEOFDAY) &&\
    (defined (__GNUC__) && (defined (__i386__) || defined (__x86_64__))))
    //  Retrieves current time in processor ticks. This function is intended
    //  for internal usage - use 'now' function instead.
    inline uint64_t now_ticks ()
    {
        uint32_t low;
        uint32_t high;
        __asm__ volatile ("rdtsc"
            : "=a" (low), "=d" (high));
        return (uint64_t) high << 32 | low;
    }
#endif

    //  Retrieves current time in nanoseconds. This function is intended
    //  for internal usage - use 'now' function instead.
    inline uint64_t now_nsecs ()
    {
        timeval tv;
        int rc = gettimeofday (&tv, NULL);
        assert (rc == 0);
        return ((time_t) tv.tv_sec) * 1000000000 + tv.tv_usec * 1000;
    }

#if (!defined (PERF_FORCE_GETTIMEOFDAY) &&\
    (defined (__GNUC__) && (defined (__i386__) || defined (__x86_64__))))
    //  Precomputes CPU frequency (in Hz). Run this function at the beginning
    //  of your program, otherwise the computation (several seconds) will occur
    //  in the middle of your program. To get rid of the CPU frequency
    //  computation altogether you can force the framework to use the value
    //  you'll supply. To do so, define PERF_CPU_FREQUENCY macro. It's value
    //  should be CPU frequency (in Hz).
    inline uint64_t estimate_cpu_frequency ()
    {
#ifdef PERF_CPU_FREQUENCY
        return (PERF_CPU_FREQUENCY);
#else
        static uint64_t cpu_frequency = 0;

        if (!cpu_frequency) {

            //  Measure frequency with busy loop
            uint64_t start_nsecs = now_nsecs ();
            uint64_t start_ticks = now_ticks ();
            for (volatile int i = 0; i != 1000000000; i ++);
            uint64_t end_nsecs = now_nsecs ();
            uint64_t end_ticks = now_ticks ();
            uint64_t ticks = end_ticks - start_ticks;
            uint64_t nsecs = end_nsecs - start_nsecs;
            uint64_t busy_frq = ticks * 1000000000 / nsecs;

            //  Measure frequency with sleep
            start_nsecs = now_nsecs ();
            start_ticks = now_ticks ();
            sleep (4);
            end_nsecs = now_nsecs ();
            end_ticks = now_ticks ();
            ticks = end_ticks - start_ticks;
            nsecs = end_nsecs - start_nsecs;
            uint64_t sleep_frq = ticks * 1000000000 / nsecs;

            //  If the two frequencies are too far apart, there's a problem
            //  somewhere - presumably CPU frequency is being lowered
            //  as a power saving measure. However, this test doesn't seem
            //  to be sufficient. It may succeed even if power saving
            //  measures are on.
            assert (busy_frq < sleep_frq * 1.25 && sleep_frq < busy_frq * 1.25);

            cpu_frequency = busy_frq;
        }

        return cpu_frequency;
#endif
    }
#endif

    //  Get current time
    inline time_instant_t now ()
    {
#if (!defined (PERF_FORCE_GETTIMEOFDAY) &&\
    (defined (__GNUC__) && (defined (__i386__) || defined (__x86_64__))))

        //  When function is called for the first time, set timestamps to zero
        //  so that they'll be recomputed below.
        static uint64_t last_nsecs = 0;
        static uint64_t last_ticks = 0;

        //  Get current time (in CPU ticks)
        uint64_t current_ticks = now_ticks ();

        //  Find out whether one second has already elapsed since the last
        //  system time measurement. If so, measure the system time anew.
        if (current_ticks - last_ticks >= estimate_cpu_frequency ()) {
            last_nsecs = now_nsecs ();
            last_ticks = now_ticks ();
            current_ticks = last_ticks;
        }

        //  Return the sum of last measured system time and the ticks
        //  elapsed since then.
        return last_nsecs + ((current_ticks - last_ticks) * 1000000000 /
            estimate_cpu_frequency ());
#else
        return now_nsecs ();
#endif
    }

}

#endif
