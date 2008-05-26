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

#ifndef __EXCHANGE_TIME_HPP_INCLUDED__
#define __EXCHANGE_TIME_HPP_INCLUDED__

#include <assert.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

namespace exchange
{

    //  Retrieves current time in processor ticks
    static uint64_t now_ticks ()
    {
        uint32_t low;
        uint32_t high;
        __asm__ volatile ("rdtsc"
            : "=a" (low), "=d" (high));
        return (uint64_t) high << 32 | low;
    }

    //  Retrieves current time in microseconds
    static uint64_t now_usec ()
    {
        timeval tv;
        int rc = gettimeofday (&tv, NULL);
        assert (rc == 0);
        return ((time_t) tv.tv_sec) * 1000000 + tv.tv_usec;
    }

    //  Returns CPU frequency (in Hz)
    //  This function takes considerable amount of time when run the first
    //  time. It's advisable to run in at the program startup. To precompute
    //  the value.
    uint64_t estimate_cpu_frequency ()
    {
        static uint64_t cpu_frequency = 0;

        if (!cpu_frequency) {

            //  Measure frequency with busy loop
            uint64_t start_usec = now_usec ();
            uint64_t start_ticks = now_ticks ();
            for (volatile int i = 0; i != 1000000000; i ++);
            uint64_t end_usec = now_usec ();
            uint64_t end_ticks = now_ticks ();
            uint64_t ticks = end_ticks - start_ticks;
            uint64_t usec = end_usec - start_usec;
            uint64_t busy_frq = ticks * 1000000 / usec;

            //  Measure frequency with sleep
            start_usec = now_usec ();
            start_ticks = now_ticks ();
            sleep (4);
            end_usec = now_usec ();
            end_ticks = now_ticks ();
            ticks = end_ticks - start_ticks;
            usec = end_usec - start_usec;
            uint64_t sleep_frq = ticks * 1000000 / usec;

            //  If the two frequencies are too far apart, there's a problem
            //  somewhere (presumably CPU frequency is being lowered
            //  as a power saving measure)
//            assert (busy_frq < sleep_frq * 1.25 && sleep_frq < busy_frq * 1.25);

            cpu_frequency = busy_frq;
        }

        return cpu_frequency;
    }

}

#endif
