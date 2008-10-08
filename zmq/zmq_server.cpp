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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
using namespace std;

#include "config.hpp"
#include "stdint.hpp"
#include "err.hpp"
#include "zmq_server.hpp"
using namespace zmq;

//  Info about a single object.
struct object_info_t
{
    string interface;
    int fd;
};

//  Maps object name to object info.
typedef map <string, object_info_t> objects_t;

//  This function is used when there is socket disconnection. It cleans all
//  the objects registered by the connection.
void unregister (int s_, objects_t *objects_)
{
    for (int type_id = 0; type_id != type_id_count; type_id ++) {
        objects_t::iterator it = objects_[type_id].begin();
        while (it != objects_[type_id].end ()) {
            if (it->second.fd == s_) {
                objects_t::iterator tmp = it;
                it ++;
                objects_[type_id].erase (tmp);
                continue;
            }
            it ++;
        }
    }
}

int main (int argc, char *argv [])
{
    //  Check command line parameters.
    if ((argc != 1 && argc != 2) || (argc == 2 &&
          strcmp (argv [1], "--help") == 0)) {
        printf ("Usage: zmq_server [port]\n");
        printf ("Default port is %d.\n", (int) default_locator_port);
        return 1;
    }

    //  Create IP address to listen to.
    sockaddr_in address;
    memset (&address, 0, sizeof (sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl (INADDR_ANY);
    address.sin_port =
        htons (argc == 1 ? default_locator_port : atoi (argv [1]));

    //  Create a listening socket.
    int listening_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    errno_assert (listening_socket != -1);

    //  Allow socket reusing.
    int flag = 1;
    int rc = setsockopt (listening_socket, SOL_SOCKET, SO_REUSEADDR,
        &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Bind the socket to the network interface and port.
    rc = bind (listening_socket, (struct sockaddr*) &address,
        sizeof (address));
    errno_assert (rc == 0);
              
    //  Listen for incomming connections.
    rc = ::listen (listening_socket, 1);
    errno_assert (rc == 0);

    //  Initialise the pollset.
    typedef vector <pollfd> pollfds_t;
    pollfds_t pollfds (1);
    pollfds [0].fd = listening_socket;
    pollfds [0].events = POLLIN;

    //  Object repository. Individual object maps are placed into slots
    //  identified by the type ID of particular object.
    objects_t objects [type_id_count];

    while (true) {

        //  Poll on the sockets.
        rc = poll (&pollfds [0], pollfds.size (), -1);
        errno_assert (rc != -1);

        //  Traverse all the sockets.
        for (pollfds_t::size_type pos = 1; pos < pollfds.size (); pos ++) {

            //  Get the socket being currently being processed.
            int s = pollfds [pos].fd;

            if ((pollfds [pos].revents & POLLERR) ||
                  (pollfds [pos].revents & POLLHUP)) {

                //  Unregister all the symbols registered by this connection
                //  and delete the descriptor from pollfds vector.
                unregister (s, objects);
                pollfds.erase (pollfds.begin () + pos);
                close (s);
                continue;
            }

            if (pollfds [pos].revents & POLLIN) {

                //  Read command ID.
                unsigned char cmd;
                unsigned char reply;
                ssize_t nbytes = recv (s, &cmd, 1, MSG_WAITALL);

                //  Connection closed by peer.
                if (nbytes == 0) {

                    //  Unregister all the symbols registered by this connection
                    //  and delete the descriptor from pollfds vector.
                    unregister (s, objects);
                    pollfds.erase (pollfds.begin () + pos);
                    close (s);
                    continue;
                }

                assert (nbytes == 1);

                switch (cmd) {
                case create_id:
                    {
                        //  Parse type ID.
                        unsigned char type_id;
                        nbytes = recv (s, &type_id, 1, MSG_WAITALL);
                        assert (nbytes == 1);

                        //  Parse object name.
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Parse interface.
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char interface [256];
                        nbytes = recv (s, interface, size, MSG_WAITALL);
                        assert (nbytes == size);
                        interface [size] = 0;

                        //  Insert object to the repository.
                        object_info_t info = {interface, s};
                        if (!objects [type_id].insert (
                              objects_t::value_type (name, info)).second) {

                            //  Send an error if the exchange already exists.
                            reply = fail_id;
                            nbytes = send (s, &reply, 1, 0);
                            assert (nbytes == 1);
#ifdef ZMQ_TRACE
                            printf ("Error when creating object: object %d:%s"
                                " already exists.\n", (int) type_id, name);
#endif
                            break;
                        }

                        //  Send reply command.
                        reply = create_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);
#ifdef ZMQ_TRACE
                        printf ("Object %d:%s created (%s).\n", type_id, name,
                            interface);
#endif
                        break;
                    }
                case get_id:
                    {
                        //  Parse type ID.
                        unsigned char type_id;
                        nbytes = recv (s, &type_id, 1, MSG_WAITALL);
                        assert (nbytes == 1);

                        //  Parse object name.
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Find the exchange in the repository.
                        objects_t::iterator it =
                            objects [type_id].find (name);
                        if (it == objects [type_id].end ()) {

                             //  Send the error.
                             reply = fail_id;
                             nbytes = send (s, &reply, 1, 0);
                             assert (nbytes == 1);
#ifdef ZMQ_TRACE
                             printf ("Error when looking for an object: "
                                 "object %d:%s does not exist.\n",
                                 (int) type_id, name);
#endif
                             break;
                        }

                        //  Send reply command.
                        reply = get_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);

                        //  Send the interface.
                        size = it->second.interface.size ();
                        nbytes = send (s, &size, 1, 0);
                        assert (nbytes == 1);
                        nbytes = send (
                            s, it->second.interface.c_str (), size, 0);
                        assert (nbytes == size);

#ifdef ZMQ_TRACE
                        printf ("Object %d:%s retrieved (%s).\n", (int) type_id,
                            name, it->second.interface.c_str ());
#endif
                        break;
                    }
                default:
                    assert (false);
                }
            }
        }

        //  Accept incoming connection.
        if (pollfds [0].revents & POLLIN) {
            int s = accept (listening_socket, NULL, NULL);
            errno_assert (s != -1);
            pollfd pfd = {s, POLLIN, 0};
            pollfds.push_back (pfd);
        }

    }

    return 0;
}

