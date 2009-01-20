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

#ifndef __PERF_SCTP_TRANSPORT_HPP_INCLUDED__
#define __PERF_SCTP_TRANSPORT_HPP_INCLUDED__

#include "i_transport.hpp"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

namespace perf
{

    class sctp_t : public i_transport
    {
    public:
        sctp_t (bool listen_, const char *ip_address_, unsigned short port_,
              bool nagle_)
        {
            s = -1;

            //  Parse the IP address.
            sockaddr_in addr;
            ::memset (&addr, 0, sizeof (addr));
            addr.sin_family = AF_INET;
            int rc = ::inet_pton (AF_INET, ip_address_, &addr.sin_addr);
            assert (rc > 0);
            addr.sin_port = htons (port_);

            if (listen_) {

                //  If 'listen' flag is set, object waits for connection
                //  initiated by the other party. 'ip_address' is interpreted
                //  as a network interface IP address (on local machine).

                //  Create a listening socket.
                listening_socket = socket (AF_INET, SOCK_STREAM,
                    IPPROTO_SCTP);
                assert (listening_socket != -1);

                //  Subscribe for SCTP events.
                sctp_event_subscribe events;
                memset (&events, 0, sizeof (events));
                events.sctp_data_io_event = 1;
                rc = setsockopt (listening_socket, IPPROTO_SCTP, SCTP_EVENTS,
                    &events, sizeof (events));
                assert (rc == 0);

                //  Bind the socket to the network interface and port.
                rc = bind (listening_socket, (struct sockaddr*) &addr,
                    sizeof (addr));
                assert (rc == 0);

                //  Listen for incomming connections.
                rc = ::listen (listening_socket, 1);
                assert (rc == 0);

                //  Accept first incoming connection.
                s = accept (listening_socket, NULL, NULL);
                assert (s != -1);
            }
            else {

                //  If 'listen' flag is not set, object actively creates
                //  a connection. ip_address is interpreted as peer's
                //  IP addess.

                //  Create a socket.
                s = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
                assert (s != -1);

                //  Subscribe for SCTP events.
                sctp_event_subscribe events;
                memset (&events, 0, sizeof (events));
                events.sctp_data_io_event = 1;
                rc = setsockopt (s, IPPROTO_SCTP, SCTP_EVENTS, &events,
                    sizeof (events));
                assert (rc == 0);

                //  Connect to the peer.
                rc = connect (s, (sockaddr*) &addr, sizeof (addr));
                assert (rc != -1);

                //  Mark listening socket as unused.
                listening_socket = -1;
            }

            //  Disable Nagle aglorithm if reuqired.
            if (!nagle_) {
                int flag = 1;
                rc = ::setsockopt (s, IPPROTO_SCTP, SCTP_NODELAY, &flag,
                    sizeof (int));
                assert (rc == 0);
            }
        }

        inline ~sctp_t ()
        {
            //  Cleanup the socket.
            int rc = ::close (s);
            assert (rc == 0);

            //  Close the listening socket.
            if (listening_socket != -1) {
                rc = ::close (listening_socket);
                assert (rc == 0);
            }
        }

        inline virtual void send (size_t size_)
        {
            //  Create the message.
            void *buffer = malloc (size_);
            assert (buffer);

            //  Send the data over the wire.
            ssize_t nbytes = sctp_sendmsg (s, buffer, size_, NULL, 0, 0, 0,
                0, 0, 0);
            assert (nbytes == (ssize_t) size_);

            //  Cleanup.
            free (buffer);
        }

        inline virtual size_t receive ()
        {
            unsigned char buffer [4096]; 
            ssize_t nbytes = sctp_recvmsg (s, buffer, sizeof (buffer),
                NULL, 0, NULL, NULL);
            assert (nbytes > 0 && nbytes < 4096);

            //  Return message size.
            return nbytes;
        }

    protected:
        int listening_socket;
        int s;
    };

}

#endif
