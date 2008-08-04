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

#include <assert.h>
#include <stdio.h>
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

#include "stdint.hpp"
#include "err.hpp"
#include "zmq_server.hpp"
using namespace zmq;

struct exchange_info_t
{
    string interface;
    uint16_t port;
    int fd;
};

typedef map <string, exchange_info_t> exchanges_t;

struct queue_info_t
{
    string interface;
    uint16_t port;
    int fd;
};

typedef map <string, queue_info_t> queues_t;

void unregister (int s_, exchanges_t &exchanges_, queues_t &queues_)
{
    //  Delete the symbols associated with socket s_ from
    //  the exchange repository
    exchanges_t::iterator eit = exchanges_.begin();
    while (eit != exchanges_.end ()) {
        if (eit->second.fd == s_) {
            exchanges_t::iterator tmp = eit;
            eit ++;
            exchanges_.erase (tmp);
            continue;
        }
        eit ++;
    }

    //  Delete the symbols associated with socket s_ from
    //  the queue repository
    queues_t::iterator qit = queues_.begin();
    while (qit != queues_.end ()) {
        if (qit->second.fd == s_) {
            queues_t::iterator tmp = qit;
            qit ++;
            queues_.erase (tmp);
            continue;
        }
        qit ++;
    }
}

int main (int argc, char *argv [])
{
    if (argc != 3) {
        printf ("usage: zmq_server <inteface> <port>\n");
        return 1;
    }

    //  Create addess
    sockaddr_in interface;
    memset (&interface, 0, sizeof (interface));
    interface.sin_family = AF_INET;
    int rc = inet_pton (AF_INET, argv [1], &interface.sin_addr);
    errno_assert (rc > 0);
    interface.sin_port = htons (atoi (argv [2]));

    //  Create a listening socket
    int listening_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    errno_assert (listening_socket != -1);

    //  Allow socket reusing
    int flag = 1;
    rc = setsockopt (listening_socket, SOL_SOCKET, SO_REUSEADDR,
        &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Bind the socket to the network interface and port
    rc = bind (listening_socket, (struct sockaddr*) &interface,
        sizeof (interface));
    errno_assert (rc == 0);
              
    //  Listen for incomming connections
    rc = ::listen (listening_socket, 1);
    errno_assert (rc == 0);

    //  Initialise the pollset
    typedef vector <pollfd> pollfds_t;
    pollfds_t pollfds (1);
    pollfds [0].fd = listening_socket;
    pollfds [0].events = POLLIN;

    //  Exchange and queue repositories
    exchanges_t exchanges;
    queues_t queues;

    while (true) {

        //  Poll on the sockets
        rc = poll (&pollfds [0], pollfds.size (), -1);
        errno_assert (rc != -1);

        //  Traverse all the sockets
        for (pollfds_t::size_type pos = 1; pos < pollfds.size (); pos ++) {

            //  Get the socket being currently being processed
            int s = pollfds [pos].fd;

            if ((pollfds [pos].revents & POLLERR) ||
                  (pollfds [pos].revents & POLLHUP)) {

                //  Unregister all the symbols registered by this connection
                //  and delete the descriptor from pollfds vector
                unregister (s, exchanges, queues);
                pollfds.erase (pollfds.begin () + pos);
                close (s);
                continue;
            }

            if (pollfds [pos].revents & POLLIN) {

                //  Read command ID
                unsigned char cmd;
                unsigned char reply;
                ssize_t nbytes = recv (s, &cmd, 1, MSG_WAITALL);

                //  Connection closed by peer
                if (nbytes == 0) {

                    //  Unregister all the symbols registered by this connection
                    //  and delete the descriptor from pollfds vector
                    unregister (s, exchanges, queues);
                    pollfds.erase (pollfds.begin () + pos);
                    continue;
                }

                assert (nbytes == 1);

                switch (cmd) {
                case create_exchange_id:
                    {
                        //  Parse exchange name
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Parse interface
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char interface [256];
                        nbytes = recv (s, interface, size, MSG_WAITALL);
                        assert (nbytes == size);
                        interface [size] = 0;

                        //  Parse port
                        uint16_t port;
                        nbytes = recv (s, &port, 2, MSG_WAITALL);
                        assert (nbytes == 2);
                        port = ntohs (port);

                        //  Insert exchange to the exchange repository
                        exchange_info_t info = {interface, port, s};
                        if (!exchanges.insert (
                              exchanges_t::value_type (name, info)).second) {

                            //  Send an error if the exchange already exists
                            reply = fail_id;
                            nbytes = send (s, &reply, 1, 0);
                            assert (nbytes == 1);
#ifdef ZMQ_DEBUG
                            printf ("Error when creating exchange: exchange %s"
                                " already exists.\n", name);
#endif
                            break;
                        }

                        //  Send reply command
                        reply = create_exchange_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);
#ifdef ZMQ_DEBUG
                        printf ("Exchange %s created (%s:%d).\n", name,
                            interface, (int) port);
#endif
                        break;
                    }
                case create_queue_id:
                    {
                        //  Parse queue name
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Parse interface
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char interface [256];
                        nbytes = recv (s, interface, size, MSG_WAITALL);
                        assert (nbytes == size);
                        interface [size] = 0;

                        //  Parse port
                        uint16_t port;
                        nbytes = recv (s, &port, 2, MSG_WAITALL);
                        assert (nbytes == 2);
                        port = ntohs (port);

                        //  Insert queue to the queue repository
                        queue_info_t info = {interface, port, s};
                        if (!queues.insert (
                              queues_t::value_type (name, info)).second) {

                            //  Send an error if the queue already exists
                            reply = fail_id;
                            nbytes = send (s, &reply, 1, 0);
                            assert (nbytes == 1);
#ifdef ZMQ_DEBUG
                            printf ("Error when creating queue: "
                                "queue %s already exists.\n", name);
#endif
                            break;
                        }

                        //  Send reply command
                        reply = create_queue_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);
#ifdef ZMQ_DEBUG
                        printf ("Queue %s created (%s:%d).\n", name,
                            interface, (int) port);
#endif        
                        break;
                    }
                case get_exchange_id:
                    {
                        //  Parse exchange name
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Find the exchange in the repository
                        exchanges_t::iterator it = exchanges.find (name);
                        if (it == exchanges.end ()) {

                             //  Send the error
                             reply = fail_id;
                             nbytes = send (s, &reply, 1, 0);
                             assert (nbytes == 1);
#ifdef ZMQ_DEBUG
                             printf ("Error when looking for an exchange: "
                                 "exchange %s does not exist.\n", name);
#endif
                             break;
                        }

                        //  Send reply command
                        reply = get_exchange_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);

                        //  Send the interface
                        size = it->second.interface.size ();
                        nbytes = send (s, &size, 1, 0);
                        assert (nbytes == 1);
                        nbytes = send (
                            s, it->second.interface.c_str (), size, 0);
                        assert (nbytes == size);

                        //  Send the port
                        uint16_t port = htons (it->second.port);
                        nbytes = send (s, &port, 2, 0);
                        assert (nbytes == 2);
#ifdef ZMQ_DEBUG
                        printf ("Exchange %s retrieved (%s:%d).\n", name,
                            it->second.interface.c_str (),
                            (int) it->second.port);  
#endif
                        break;
                    }
                case get_queue_id:
                    {
                        //  Parse queue name
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Find the queue in the repository
                        queues_t::iterator it = queues.find (name);
                        if (it == queues.end ()) {

                             //  Send the error
                             reply = fail_id;
                             nbytes = send (s, &reply, 1, 0);
                             assert (nbytes == 1);
#ifdef ZMQ_DEBUG
                             printf ("Error when looking for a queue: "
                                 "queue %s does not exist.\n", name);
#endif
                             break;
                        }

                        //  Send the reply command
                        reply = get_queue_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);

                        //  Send the interface
                        size = it->second.interface.size ();
                        nbytes = send (s, &size, 1, 0);
                        assert (nbytes == 1);
                        nbytes = send (
                            s, it->second.interface.c_str (), size, 0);
                        assert (nbytes == size);

                        //  Send the port
                        uint16_t port = htons (it->second.port);
                        nbytes = send (s, &port, 2, 0);
                        assert (nbytes == 2);
#ifdef ZMQ_DEBUG
                        printf ("Queue %s retrieved (%s:%d).\n", name,
                        it->second.interface.c_str (), (int) it->second.port);  
#endif
                        break;
                    }
                default:
                    assert (false);
                }
            }
        }

        //  Accept incoming connection
        if (pollfds [0].revents & POLLIN) {
            int s = accept (listening_socket, NULL, NULL);
            errno_assert (s != -1);
            pollfd pfd = {s, POLLIN, 0};
            pollfds.push_back (pfd);
        }

    }

    return 0;
}

