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

#ifndef __PERF_TCP_TRANSPORT_HPP_INCLUDED__
#define __PERF_TCP_TRANSPORT_HPP_INCLUDED__


#include "i_transport.hpp"

#include <zmq/platform.hpp>
#include <zmq/tcp_listener.hpp>
#include <zmq/tcp_socket.hpp>
#include <zmq/wire.hpp>
#include <zmq/err.hpp>

#include <assert.h>
#include <string.h>

namespace perf
{

    class tcp_t : public i_transport
    {
    public:
        tcp_t (bool listen_, const char *iface_or_host_) : tcp_listener (NULL),
              tcp_socket (NULL)
        {
#ifdef ZMQ_HAVE_WINDOWS

    //  Intialise Windows sockets. Note that WSAStartup can be called multiple
    //  times given that WSACleanup will be called for each WSAStartup.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    errno_assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 || HIBYTE (wsa_data.wVersion) == 2);
#endif  

            //  We need to know port to listen and to bind to.
            assert (strchr (iface_or_host_, ':'));
           
            if (listen_) {

                //  If 'listen' flag is set, object waits for connection
                //  initiated by the other party.

                //  Create listening socket.
                tcp_listener = new zmq::tcp_listener_t (iface_or_host_, true);

                //  Wait for and accept first incoming connection.
                tcp_socket = new zmq::tcp_socket_t (tcp_listener->accept (),
                    true);

            } else {

                //  If 'listen' flag is not set, object actively creates
                //  a connection.
               
                //  Create a socket and connect to the peer.
                tcp_socket = new zmq::tcp_socket_t (iface_or_host_, true);
            }

        }

        inline ~tcp_t ()
        {
            //  Close sockets.
            delete tcp_socket;
            if (tcp_listener)
                delete tcp_listener;
        }

        inline virtual void send (size_t size_)
        {
        
            //  Create the message.
            void *buffer = malloc (sizeof (uint32_t) + size_);
            assert (buffer);
            zmq::put_uint32 ((unsigned char*)buffer, size_);
            
            //  Send the data over the wire.

            int bytes = tcp_socket->write (buffer, sizeof (uint32_t) + size_);
            assert (bytes == (int) (sizeof (uint32_t) + size_));

            //  Cleanup.
            free (buffer);
        }

        inline virtual size_t receive ()
        {
            //  Read the message size.
            uint32_t sz;

            int bytes = tcp_socket->read (&sz, sizeof (uint32_t));
            assert (bytes == sizeof (uint32_t));
            
            sz = zmq::get_uint32 ((unsigned char*)&sz);
    
            //  Allocate the buffer to read the message.
            void *buffer = malloc (sz);
            assert (buffer);

            //  Read the message body.
            bytes = tcp_socket->read (buffer, sz);
            assert (bytes == (int) sz);

            //  Cleanup.
            free (buffer);

            //  Return message size.
            return bytes;
        }

    protected:
        zmq::tcp_listener_t *tcp_listener;
        zmq::tcp_socket_t *tcp_socket;
    };

}

#endif
