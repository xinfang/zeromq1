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

#ifndef __PERF_PING_PONG_HPP_INCLUDED__
#define __PERF_PING_PONG_HPP_INCLUDED__

#include "../interfaces/i_worker.hpp"
#include "../helpers/time.hpp"

#include <stdio.h>

namespace perf
{

    class ping_pong_t : public i_worker
    {
    public:
        inline ping_pong_t (int roundtrip_count, size_t message_size) :
            roundtrip_count (roundtrip_count), message_size (message_size)
        {
        }

        inline virtual void run (i_transport &transport,
            const char *prefix = NULL)
        {
            //  Allocate the array for performance results
            time_instant_t *out_times = (time_instant_t*) ::malloc (
                roundtrip_count * sizeof (time_instant_t));
            assert (out_times);

            time_instant_t *in_times = (time_instant_t*) ::malloc (
                roundtrip_count * sizeof (time_instant_t));
            assert (in_times);

            //  Pass the message there and forth, record the times of each
            //  message arrival
            for (int roundtrip_nbr = 0; roundtrip_nbr != roundtrip_count;
                  roundtrip_nbr++) {
                out_times [roundtrip_nbr] = now ();
                transport.send (message_size);
                transport.receive ();
                in_times [roundtrip_nbr] = now();
            }

            //  Save the latencies for further processing
            char filename [256];
            if (!prefix)
                prefix = "";
            ::snprintf (filename, 256, "%sin.dat", prefix);
            FILE *output_in = ::fopen (filename, "w");
            assert (output_in);

            ::snprintf (filename, 256, "%sout.dat", prefix);
            FILE *output_out = ::fopen (filename, "w");
            assert (output_out);

            for (int roundtrip_nbr = 0; roundtrip_nbr != roundtrip_count;
                  roundtrip_nbr++) {
                ::fprintf (output_in, "%llu\n", in_times [roundtrip_nbr]);
                ::fprintf (output_out, "%llu\n", out_times [roundtrip_nbr]);
            }

            //  Cleanup
            ::fclose (output_in);
            ::fclose (output_out);
            ::free (in_times);
            ::free (out_times);
        }

    protected:
        int roundtrip_count;
        size_t message_size;
    };

};

#endif
