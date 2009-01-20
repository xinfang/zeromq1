/*
    Copyright (c) 2007-2009 FastMQ Inc.

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

#ifndef __PERF_TIME_HPP_INCLUDED__
#define __PERF_TIME_HPP_INCLUDED__

#include <zmq/platform.hpp>

#include <assert.h>
#include <stdlib.h>

#ifdef ZMQ_HAVE_WINDOWS
#include <sys/types.h>
#include <sys/timeb.h>
#include <zmq/windows.hpp>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <iostream>
#include <zmq/stdint.hpp>

namespace perf
{

    //  Time instance in nanoseconds.
    //  For measurement purposes the exact point when the timer started
    //  (e.g. midnight January 1, 1970) is irrelevant. The only requirement
    //  is that all times are measured from the same starting point.
    typedef uint64_t time_instant_t;

    //  Retrieves current time in nanoseconds. This function is intended
    //  for internal usage - use 'now' function instead.
    inline uint64_t now_nsecs ()
    {
#if defined ZMQ_HAVE_WINDOWS
        SYSTEMTIME st;
        FILETIME ft;

        GetSystemTime (&st);
        SystemTimeToFileTime (&st, &ft);

        //  FILETIME contains a 64-bit value representing the number of
        //  100-nanosecond intervals since January 1, 1601 (UTC).
        uint64_t tmp = ((uint64_t) ft.dwHighDateTime << 32 | ft.dwLowDateTime);

        //  ~369 years of shift to get from 1601 to POSIX epoch (1970)
        const uint64_t shift = uint64_t (369 * 365 * 24) * uint64_t (60 * 60) *
            uint64_t (1000 * 1000 * 10);

        //  Adjust the epoch and convert to nanoseconds.
        return (tmp - shift) * 100;
#else
        struct timeval tv;
        int rc = gettimeofday (&tv, NULL);
        assert (rc == 0);
        return tv.tv_sec * (uint64_t) 1000000000 + tv.tv_usec * 1000;
#endif
    }

#if (defined (__GNUC__) && (defined (__i386__) || defined (__x86_64__)))
    //  Precomputes CPU frequency (in Hz). If ferquency measured in busy loop
    //  is too far (more than +- 10%) away from freq measured in sleep 
    //  function returns 0.
    inline uint64_t estimate_cpu_frequency ()
    {
        uint64_t cpu_frequency = 0;

        uint32_t low;
        uint32_t high;

        //  Measure frequency with busy loop.
        uint64_t start_nsecs = now_nsecs ();
        __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
        uint64_t start_ticks = (uint64_t) high << 32 | low;

        for (volatile int i = 0; i != 1000000000; i ++);

        uint64_t end_nsecs = now_nsecs ();
        __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
        uint64_t end_ticks = (uint64_t) high << 32 | low;

        uint64_t ticks = end_ticks - start_ticks;
        uint64_t nsecs = end_nsecs - start_nsecs;
        uint64_t busy_frq = ticks * 1000000000 / nsecs;

        std::cout << "CPU frequency measured with busy loop: " << busy_frq 
            << " [Hz]" << std::endl;
        
        //  Measure frequency with sleep.
        start_nsecs = now_nsecs ();
        __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
        start_ticks = (uint64_t) high << 32 | low;

#ifdef ZMQ_HAVE_WINDOWS
        Sleep (4000);
#else
        usleep (4000000);
#endif

        end_nsecs = now_nsecs ();
        __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
        end_ticks = (uint64_t) high << 32 | low;
        ticks = end_ticks - start_ticks;
        nsecs = end_nsecs - start_nsecs;
        uint64_t sleep_frq = ticks * 1000000000 / nsecs;

        std::cout << "CPU frequency measured with sleep: " << sleep_frq 
            << " [Hz]" << std::endl << std::endl;

        //  If the two frequencies are too far apart, there's a problem
        //  somewhere - presumably CPU frequency is being lowered
        //  as a power saving measure. However, this test doesn't seem
        //  to be sufficient. It may succeed even if power saving
        //  measures are on.
        if (busy_frq > sleep_frq * 1.1 && sleep_frq < busy_frq * 1.1) {
            std::cerr << "Difference more than +-10%!" << std::endl;
            std::cerr << "It looks like while your CPU is in idle, frequency "
                << "is being lowered. Probably \nby some power saving "
                << "enhancement. Please turn off all cpu frequency "
                << "lowering\nleverages and rerun the program." << std::endl;
            return 0;
        }

        // Return average from busy and sleep frequency.
        cpu_frequency = (busy_frq + sleep_frq) / 2;

        return cpu_frequency;
    }
#endif

    //  Get current time in nanosecond resolution.
    inline time_instant_t now ()
    {
#if (defined (__GNUC__) && defined (PERF_CPU_FREQUENCY) && \
    (defined (__i386__) || defined (__x86_64__)))

        //  When function is called for the first time, set timestamps to zero
        //  so that they'll be recomputed below.
        static uint64_t last_nsecs = 0;
        static uint64_t last_ticks = 0;

        uint32_t low;
        uint32_t high;

        //  Get current time (in CPU ticks).
        __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
        uint64_t current_ticks = (uint64_t) high << 32 | low;

        //  Find out whether one second has already elapsed since the last
        //  system time measurement. If so, measure the system time anew.
        if (current_ticks - last_ticks >= PERF_CPU_FREQUENCY) {
            last_nsecs = now_nsecs ();
            __asm__ volatile ("rdtsc"
                : "=a" (low), "=d" (high));
            last_ticks = (uint64_t) high << 32 | low;
            current_ticks = last_ticks;
        }

        //  Return the sum of last measured system time and the ticks
        //  elapsed since then.
        return last_nsecs + ((current_ticks - last_ticks) * 1000000000 /
            PERF_CPU_FREQUENCY);

#elif defined ZMQ_HAVE_WINDOWS

        //  When function is called for the first time, set timestamps to zero
        //  so that they'll be recomputed below.
        static uint64_t last_nsecs = 0;
        static LARGE_INTEGER last_ticks;
        static LARGE_INTEGER ticks_per_second;

        //  First run.
        if (last_nsecs == 0) {
            last_ticks.LowPart = 0;
            last_ticks.HighPart = 0;

            ticks_per_second.LowPart = 0;
            ticks_per_second.HighPart = 0;
        }

        if (ticks_per_second.QuadPart == 0) {

            //  Get the high resolution counter's accuracy.
            QueryPerformanceFrequency (&ticks_per_second);
        }

        //  Get current time (in CPU ticks).
        LARGE_INTEGER current_ticks;
        QueryPerformanceCounter (&current_ticks);

        //  Find out whether one second has already elapsed since the last
        //  system time measurement. If so, measure the system time anew.
        if (last_nsecs == 0 || current_ticks.QuadPart - last_ticks.QuadPart >= 
              ticks_per_second.QuadPart) {
            last_nsecs = now_nsecs ();
            QueryPerformanceCounter (&last_ticks);
            current_ticks = last_ticks;
        }

        //  Return the sum of last measured system time and the ticks
        //  elapsed since then.
        double ns_per_tick = double (1000000000) /
            double (ticks_per_second.QuadPart);

        return last_nsecs + (uint64_t) ((current_ticks.QuadPart -
            last_ticks.QuadPart) * ns_per_tick);

#else
        return now_nsecs ();
#endif
    }
}
#endif
