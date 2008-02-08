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

#ifndef __PERF_RAW_RECEIVER_HPP_INCLUDED__
#define __PERF_RAW_RECEIVER_HPP_INCLUDED__

#include "../interfaces/i_worker.hpp"
#include "../helpers/time.hpp"

#include <stdio.h>

namespace perf
{

    class raw_receiver_t : public i_worker
    {
    public:
        inline raw_receiver_t (int message_count_) :
            message_count (message_count_)
        {
        }

        inline virtual void run (i_transport &transport_,
            const char *prefix_ = NULL)
        {

            time_instant_t start_time = 0;

            //  Receive the messages as quickly as possible
            for (int message_nbr = 0; message_nbr != message_count;
                  message_nbr++){
                transport_.receive ();
                 if (message_nbr == 0)
                    start_time = now();
            }

            time_instant_t stop_time = now();

            //  Save the results
            char filename [256];
            if (!prefix_)
                prefix_ = "";
            ::snprintf (filename, 256,
                "%sin.dat", prefix_);
            FILE *output = ::fopen (filename, "w");
            assert (output);
            ::fprintf (output, "%llu\n", start_time);
            ::fprintf (output, "%llu\n", stop_time);
            ::fclose (output);  
        }

    protected:
        int message_count;
    };

}

#endif
