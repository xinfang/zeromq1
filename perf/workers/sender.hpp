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

#ifndef __PERF_SENDER_HPP_INCLUDED__
#define __PERF_SENDER_HPP_INCLUDED__

#include "../interfaces/i_worker.hpp"
#include "../helpers/time.hpp"

#include <stdio.h>

namespace perf
{

    class sender_t : public i_worker
    {
    public:
        inline sender_t (int message_count, size_t message_size) :
            message_count (message_count), message_size (message_size)
        {
        }

        inline virtual void run (i_transport &transport,
            const char *prefix = NULL)
        {
            //  Allocate the array for performance results
            time_instant_t *out_times = (time_instant_t*) ::malloc (
                message_count * sizeof (time_instant_t));
            assert (out_times);

            //  Send the messages as quickly as possible
            for (int message_nbr = 0; message_nbr != message_count;
                  message_nbr++) {
                out_times [message_nbr] = now ();
                transport.send (message_size);
            }

            //  Save the sender cycle times
            char filename [256];
            if (!prefix)
                prefix = "";
            ::snprintf (filename, 256,
                "%sout.dat", prefix);
            FILE *output = ::fopen (filename, "w");
            assert (output);
            for (int message_nbr = 0; message_nbr != message_count;
                  message_nbr++)
                ::fprintf (output, "%llu\n", out_times [message_nbr]);
            ::fclose (output);
            ::free (out_times);
            sleep (1);
        }

    protected:
        int message_count;
        size_t message_size;
    };

}

#endif
