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

#ifndef __PERF_RECEIVER_HPP_INCLUDED__
#define __PERF_RECEIVER_HPP_INCLUDED__

#include "../interfaces/i_worker.hpp"
#include "../helpers/time.hpp"

#include <stdio.h>

namespace perf
{

    class receiver_t : public i_worker
    {
    public:
        inline receiver_t (int message_count) :
            message_count (message_count)
        {
        }

        inline virtual void run (i_transport &transport,
            const char *prefix = NULL)
        {
            //  Allocate the array for performance results
            time_instant_t *in_times = (time_instant_t*) ::malloc (
                message_count * sizeof (time_instant_t));
            assert (in_times);

            //  Receive the messages as quickly as possible
            for (int message_nbr = 0; message_nbr != message_count;
                  message_nbr++) {
                //  The time is measured *after* we get the message
                //  That way we avoid the peak for first message (waiting
                //  while sender application is started)
                transport.receive ();
                in_times [message_nbr] = now ();
            }

            //  Save the receiver throughputs
            char filename [256];
            if (!prefix)
                prefix = "";
            ::snprintf (filename, 256,
                "%sin.dat", prefix);
            FILE *output = ::fopen (filename, "w");
            assert (output);
            for (int message_nbr = 0; message_nbr != message_count;
                  message_nbr++)
                ::fprintf (output, "%llu\n", in_times [message_nbr]);
            ::fclose (output);
            ::free (in_times);
        }

    protected:
        int message_count;
    };

}

#endif
