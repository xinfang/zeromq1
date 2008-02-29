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

#ifndef __PERF_FILES_HPP_INCLUDED__
#define __PERF_FILES_HPP_INCLUDED__

namespace perf
{
    // reads two lines from file
    void read_times_1f (perf::time_instant_t *start_time_,
        perf::time_instant_t *stop_time_, const char *filename_)
    {
        //  Load the results
        FILE *input = ::fopen (filename_, "r");
        assert (input);
        fscanf (input, "%llu", start_time_);
        fscanf (input, "%llu", stop_time_);
        fclose (input);
    }

    // reads one line from two files
    void read_times_2f (perf::time_instant_t *start_time_,
        perf::time_instant_t *stop_time_, const char *prefix_)
    {
        //  Load the results
        char filename [256];
        snprintf (filename, 256, "%sout.dat", prefix_);
        FILE *input = ::fopen (filename, "r");
        assert (input);
        fscanf (input, "%llu", start_time_);
        fclose (input);
        
        snprintf (filename, 256, "%sin.dat", prefix_);
        input = ::fopen (filename, "r");
        assert (input);
        fscanf (input, "%llu", stop_time_);
        fclose (input);
    }                                              
}

#endif
