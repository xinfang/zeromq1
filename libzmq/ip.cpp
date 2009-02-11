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

#include <cstdlib>
#include <assert.h>
#include <string.h>
#include <string>
#include <stdlib.h>

#include <zmq/ip.hpp>
#include <zmq/err.hpp>
#include <zmq/stdint.hpp>

void zmq::resolve_ip_interface (sockaddr_in* addr_, char const *interface_)
{
    //  Find the ':' that separates NIC name from port.
    const char *delimiter = strchr (interface_, ':');
    assert (delimiter);

    //  Clean the structure and fill in protocol family.
    memset (addr_, 0, sizeof (sockaddr_in));
    addr_->sin_family = AF_INET;

    //  Copy the interface to a new buffer.
    //  TODO: Consider using dynamically allocated buffer.
    assert (delimiter - interface_ < 256);
    char buf [256];
    memcpy (buf, interface_, delimiter - interface_);
    buf [delimiter - interface_] = 0;
    
    //  Convert IP address into sockaddr_in structure.
    int rc = inet_pton (AF_INET, buf, addr_);
    assert (rc != 0);
    errno_assert (rc == 1);

    //  Resolve the port.
    addr_->sin_port = htons ((uint16_t) atoi (delimiter + 1));
}

void zmq::resolve_ip_hostname (sockaddr_in *addr_, const char *hostname_)
{
    //  Find the ':' that separates hostname name from port.
    const char *delimiter = strchr (hostname_, ':');
    assert (delimiter);

    //  Copy the hostname to a new buffer.
    //  TODO: Consider using dynamically allocated buffer.
    assert (delimiter - hostname_ < 256);
    char hostname [256];
    memcpy (hostname, hostname_, delimiter - hostname_);
    hostname [delimiter - hostname_] = 0;

    //  Resolve host name.
    addrinfo req;
    memset (&req, 0, sizeof (req));
    req.ai_family = AF_INET;
    addrinfo *res;
    int rc = getaddrinfo (hostname, NULL, &req, &res);
    gai_assert (rc);
    assert (res->ai_addr->sa_family == AF_INET);
    memcpy (addr_, res->ai_addr, sizeof (sockaddr_in));
    freeaddrinfo (res);
    
    //  Fill in the port number.
    addr_->sin_port = htons ((uint16_t) atoi (delimiter + 1));
}
