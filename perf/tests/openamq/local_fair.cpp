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

#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#include "../../transports/openamq.hpp"

int main (int argc, char *argv [])
{
    if (argc != 6) {
        cerr << "Usage: local_fair <hostname> <direct 0/1> <message size> "
            "<message count> <number of consumers>" << endl;
        return 1;
    }

    //  Parse & print command line arguments.
    const char *host = argv [1];
    bool direct = atoi (argv [2]);
    size_t message_size = atoi (argv [3]);
    int message_count = atoi (argv [4]);
    int consumer_count = atoi (argv [5]);

    cout << "direct: " << direct << endl;
    cout << "message size: " << message_size << " [B]" << endl;
    cout << "message count: " << message_count << endl;
    cout << "number of consumers: " << consumer_count << endl;

    //  Create connection to OpenAMQ broker.
    perf::openamq_t transport (host, "cmds", "msgs", direct);

    //  Wait till all the consumers are alive and running.
    for (int consumer_nbr = 0; consumer_nbr < consumer_count; consumer_nbr++)
        transport.receive ();

    //  Send all the messages as fast as possible.
    for (int message_nbr = 0; message_nbr != message_count; message_nbr ++)
        transport.send (message_size);

    sleep (100);

    return 0;
}
