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
#include <cstdio>
#include <string>
#include <limits>

#include "../../transports/zmq.hpp"
#include "../../workers/raw_receiver.hpp"

#include "./test.hpp"


int main (int argc, char *argv [])
{
    if (argc != 1){
        printf ("Usage: local\n");
        return 1;
    }

    perf::zmq_t transport (true, "0.0.0.0", PORT_NUMBER);
    perf::raw_receiver_t worker (1000000);
    worker.run (transport, "", 0);
    return 0;
}

