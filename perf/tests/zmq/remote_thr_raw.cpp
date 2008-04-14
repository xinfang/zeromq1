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

#include "../../transports/zmq.hpp"
#include "../../workers/raw_sender.hpp"

#include "./test.hpp"


int main (int argc, char *argv [])
{

    if (argc != 2) {
        printf ("Usage: remote <ip address where \'local\' runs>\n");
        return 1;
    }

    perf::zmq_t transport (false, argv [1], PORT_NUMBER);
    perf::raw_sender_t worker (1000000, 6);
    worker.run (transport, "", 0);
           
    return 0;
}

void *worker_function (void *args_)
{
    // args struct
    worker_args_t *w_args = (worker_args_t*)args_;

    char prefix [20];
    memset (prefix, '\0', sizeof (prefix));
    snprintf (prefix, sizeof (prefix) - 1, "%i_%i_", w_args->msg_size,
        w_args->id);


    delete w_args;

    return NULL;
}
