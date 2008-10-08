/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

#include "../../helpers/time.hpp"
#include "../../transports/openamq.hpp"

int main (int argc, char *argv [])
{
    if (argc != 5) {
        cerr << "Usage: local_fair <hostname> <direct 0|1> <message size> "
            "<message count>" << endl;
        return 1;
    }

    //  Parse & print command line arguments.
    const char *host = argv [1];
    bool direct = atoi (argv [2]);
    size_t message_size = atoi (argv [3]);
    int message_count = atoi (argv [4]);

    cout << "direct: " << direct << endl;
    cout << "message size: " << message_size << " [B]" << endl;
    cout << "message count: " << message_count << endl;

    //  Create connection to OpenAMQ broker.
    perf::openamq_t transport (host, "msgs", "cmds", direct);

    //  Send notification to local that this instance of remote is running.
    transport.send (1);

    //  Receive all the remaining messages. Compute longest interval between
    //  two subsequent messages.
    uint64_t max_interval = 0;
    perf::time_instant_t last_time = 0;
    for (int message_nbr = 0; message_nbr != message_count; message_nbr ++) {
        size_t size = transport.receive ();
        assert (size == message_size);
        perf::time_instant_t current_time = perf::now ();
        uint64_t current_interval = current_time - last_time;
        if (last_time &&  current_interval > max_interval)
            max_interval = current_interval;
        last_time = current_time;
	printf ("%d\n", message_nbr);
    }

    cout << "Maximal interval between two subsequent messages: "
        << (max_interval / 1000) << " [us]" << endl;

    return 0;
}
