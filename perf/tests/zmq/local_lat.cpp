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

#include <cstdio>

#include "../../transports/zmq.hpp"
#include "../scenarios/lat.hpp"

using namespace perf;

int main (int argc, char *argv [])
{
    if (argc != 7){
        printf ("Usage: local <global_locator IP> <global_locator port> "
        "<listen IP> <listen port> <message size> <roundtrip count>\n");
        return 1;
    }

    printf ("message size: %i\n", atoi (argv [5]));
    printf ("roundtrip count: %i\n", atoi (argv [6]));

    zmq_t transport (false, "QIN", "EOUT", argv [1], atoi (argv [2]), argv [3],
        atoi (argv [4]));

    local_lat (&transport, atoi (argv [5]), atoi (argv [6]));

    return 0;
}
