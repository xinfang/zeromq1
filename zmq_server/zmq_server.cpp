/*
    Copyright (c) 2007-2009 FastMQ Inc.

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
#include <fstream>
#include <sstream>

#include <zmq/tcp_socket.hpp>
#include <zmq/tcp_listener.hpp>
#include <zmq/wire.hpp>

int main ()
{
    //  Start listening for incoming connections.
    zmq::tcp_listener_t listener ("0.0.0.0:5682");

    //  Get the configuration data to the memory.
    std::ifstream file ("dir.xml");
    assert (!file.fail ());
    std::string data;
    while (!file.eof ()) {
        std::string line;
        std::getline (file, line);
        data += line;
    }

    while (true) {

        //  Accept new connection.
        zmq::tcp_socket_t socket (listener, true);

        //  Push the configuration data to the connection.
        unsigned char buff [9];
        buff [0] = 0xff;
        zmq::put_uint64 (buff + 1, data.size ());
        socket.write (buff, 9);
        socket.write (data.c_str (), data.size ());
    }

    return 0;
}
