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
    
    if (argc != 8) {
        printf ("Usage: local_fo <global_locator IP> <global_locator port> "
            "<listen IP> <listen port> <message size> <message count> "
            "<number of subscribers>\n");
        return 1;
    }

    size_t msg_size = atoi (argv [5]);
    int roundtrip_count = atoi (argv [6]);
    int subs_count = atoi (argv [7]);

    printf ("subcribers: %i\n", subs_count);
    printf ("message size: %i\n", (int)msg_size);
    printf ("roundtrip count: %i\n", roundtrip_count);

    perf::zmq_t transport (false, "QIN", "EOUT", argv [1], atoi (argv [2]), 
        argv [3], atoi (argv [4]));
   
    perf::local_fo (&transport, msg_size, roundtrip_count, subs_count);

    return 0;
}
