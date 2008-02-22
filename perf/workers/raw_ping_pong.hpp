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

#ifndef __PERF_RAW_PING_PONG_HPP_INCLUDED__
#define __PERF_RAW_PING_PONG_HPP_INCLUDED__

#include "../interfaces/i_worker.hpp"
#include "../helpers/time.hpp"

#include <stdio.h>

namespace perf
{

    class raw_ping_pong_t : public i_worker
    {
    public:
        inline raw_ping_pong_t (int roundtrip_count_, size_t message_size_) :
            roundtrip_count (roundtrip_count_), message_size (message_size_)
        {
        }

        inline virtual void run (i_transport &transport_,
            const char *prefix_ = NULL, unsigned int thread_id_ = 0)
        {

            time_instant_t start_time = now();

            //  Pass the message there and forth, record the times of each
            //  message arrival
            for (int roundtrip_nbr = 0; roundtrip_nbr != roundtrip_count;
                  roundtrip_nbr++) {
                transport_.send (message_size, thread_id_);
                transport_.receive (thread_id_);
            }

            time_instant_t stop_time = now();

            //  Save the latencies for further processing
            char filename [256];
            if (!prefix_)
                prefix_ = "";
            ::snprintf (filename, 256, "%sin.dat", prefix_);
            FILE *output_in = ::fopen (filename, "w");
            assert (output_in);

            ::fprintf (output_in, "%llu\n", stop_time);

            ::snprintf (filename, 256, "%sout.dat", prefix_);
            FILE *output_out = ::fopen (filename, "w");
            assert (output_out);
            
            ::fprintf (output_out, "%llu\n", start_time);

            //  Cleanup
            ::fclose (output_in);
            ::fclose (output_out);
        }

    protected:
        int roundtrip_count;
        size_t message_size;
    };

}

#endif
