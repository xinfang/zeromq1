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

#include <cstdlib>
#include "../../transports/zmq.hpp"
#include "../scenarios/fo.hpp"

int main (int argc, char *argv [])
{
    if (argc != 6) {
        printf ("Usage: remote_fo <global_locator IP> <global_locator port> "
            "<message size> <message count> <subscriber id>\n");
        return 1;
    }

    const char *g_locator = argv [1];
    unsigned short g_locator_port = atoi (argv [2]);

    size_t msg_size = atoi (argv [3]);
    int roundtrip_count = atoi (argv [4]);
    const char *subs_id = argv [5];


    printf ("subscriber ID: %s\n", subs_id);
    printf ("message size: %i\n", (int)msg_size);
    printf ("roundtrip count: %i\n", roundtrip_count);

    printf ("estimating CPU frequency...\n");
    uint64_t frq = perf::estimate_cpu_frequency ();
    printf ("your CPU frequncy is %.2f GHz\n", ((double) frq) / 1000000000);

    perf::zmq_t transport (true, "QIN", "EOUT", g_locator, g_locator_port, NULL, 0);

    perf::remote_fo (&transport, msg_size, roundtrip_count, subs_id);

}
