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

#include <cstdlib>
#include <assert.h>
#include <string.h>
#include <string>
#include <stdlib.h>

#include "ip.hpp"
#include "err.hpp"
#include "stdint.hpp"

#if defined ZMQ_HAVE_SOLARIS

#include <sys/sockio.h>
#include <net/if.h>

//  On Solaris platform, network interface name can be queried by ioctl.
void zmq::resolve_nic_name (in_addr* addr_, char const *interface_)
{
    //  Create a socket.
    int fd = socket (AF_INET, SOCK_DGRAM, 0);
    assert (fd != -1);

    //  Retrieve number of interfaces.
    lifnum ifn;
    ifn.lifn_family = AF_UNSPEC;
    ifn.lifn_flags = 0;
    int rc = ioctl (fd, SIOCGLIFNUM, (char*) &ifn);
    assert (rc != -1);

    //  Allocate memory to get interface names.
    size_t ifr_size = sizeof (struct lifreq) * ifn.lifn_count;
    char *ifr = (char*) malloc (ifr_size);
    errno_assert (ifr);
    
    //  Retrieve interface names.
    lifconf ifc;
    ifc.lifc_family = AF_UNSPEC;
    ifc.lifc_flags = 0;
    ifc.lifc_len = ifr_size;
    ifc.lifc_buf = ifr;
    rc = ioctl (fd, SIOCGLIFCONF, (char*) &ifc);
    assert (rc != -1);

    //  Find the interface with the specified name and AF_INET family.
    bool found = false;
    lifreq *ifrp = ifc.lifc_req;
    for (int n = 0; n < (int) (ifc.lifc_len / sizeof (lifreq));
          n ++, ifrp ++) {
        if (!strcmp (interface_, ifrp->lifr_name)) {
            rc = ioctl (fd, SIOCGLIFADDR, (char*) ifrp);
            assert (rc != -1);
            if (ifrp->lifr_addr.ss_family == AF_INET) {
                *addr_ = ((sockaddr_in*) &ifrp->lifr_addr)->sin_addr;
                found = true;
                break;
            }
        }
    }

    //  Clean-up.
    free (ifr);
    close (fd);

    //  If interface was not found among interface names, we assume it's
    //  specified in the form of IP address.
    if (!found) {
        rc = inet_pton (AF_INET, interface_, addr_);
        assert (rc != 0);
        errno_assert (rc == 1);
    }
}

#elif defined ZMQ_HAVE_WINDOWS

void zmq::resolve_nic_name (in_addr* addr_, char const *interface_)
{
    //  TODO: Add code that'll convert interface name to IP address
    //  on Windows platform here.
    in_addr addr;
    int addr_length = sizeof (addr);
    ((sockaddr_in*) addr_)->sin_family = AF_INET;
   
    addr.S_un.S_addr  = inet_addr ((const char *) interface_);
        
    win_assert (addr.S_un.S_addr != INADDR_ANY || addr.S_un.S_addr != INADDR_NONE);
    *addr_ = addr;
}

#elif (defined ZMQ_HAVE_LINUX || defined ZMQ_HAVE_FREEBSD ||\
    defined ZMQ_HAVE_OSX || defined ZMQ_HAVE_OPENBSD ||\
    defined ZMQ_HAVE_QNXNTO)

#include <ifaddrs.h>

//  On some platforms (Linux, FreeBSD & Mac OS X), network interface name
//  can be queried using getifaddrs function.
void zmq::resolve_nic_name (in_addr* addr_, char const *interface_)
{
    //  Initialuse the output parameter.
    memset (addr_, 0, sizeof (in_addr));

    //  Get the addresses.
    ifaddrs* ifa = NULL;
    int rc = getifaddrs (&ifa);
    assert (rc == 0);    
    assert (ifa != NULL);

    //  Find the corresponding network interface.
    bool found = false;
    for (ifaddrs *ifp = ifa; ifp != NULL ;ifp = ifp->ifa_next)
        if (ifp->ifa_addr && ifp->ifa_addr->sa_family == AF_INET 
            && !strcmp (interface_, ifp->ifa_name)) 
        {
            *addr_ = ((sockaddr_in*) ifp->ifa_addr)->sin_addr;
            found = true;
        }

    //  Clean-up;
    freeifaddrs (ifa);

    //  If interface was not found among interface names, we assume it's
    //  specified in the form of IP address.
    if (!found) {
        rc = inet_pton (AF_INET, interface_, addr_);
        assert (rc != 0);
        errno_assert (rc == 1);
    }
}

#else

//  On other platforms interface name is interpreted as raw IP address.
void zmq::resolve_nic_name (in_addr* addr_, char const *interface_)
{
    //  Convert IP address into sockaddr_in structure.
    int rc = inet_pton (AF_INET, interface_, addr_);
    assert (rc != 0);
    errno_assert (rc == 1);
}

#endif

void zmq::resolve_ip_interface (sockaddr_in* addr_, char const *interface_)
{
    //  Find the ':' that separates NIC name from port.
    const char *delimiter = strchr (interface_, ':');

    //  Clean the structure and fill in protocol family.
    memset (addr_, 0, sizeof (sockaddr_in));
    addr_->sin_family = AF_INET;

    //  Resolve the name of the NIC.
    if (!delimiter)
        resolve_nic_name (&addr_->sin_addr, interface_);
    else {
        assert (delimiter - interface_ < 256);
        char buf [256];
        memcpy (buf, interface_, delimiter - interface_);
        buf [delimiter - interface_] = 0;
        resolve_nic_name (&addr_->sin_addr, buf);
    }

    //  Resolve the port.
    addr_->sin_port =
        htons ((uint16_t) (delimiter ? atoi (delimiter + 1) : 0));
}

void zmq::resolve_ip_hostname (sockaddr_in *addr_, const char *hostname_)
{
    //  Find the ':' that separates hostname name from port.
    const char *delimiter = strchr (hostname_, ':');
    assert (delimiter);

    //  Get the hostname.
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
    *addr_ = *((sockaddr_in *) res->ai_addr);
    freeaddrinfo (res);
    
    //  Fill in the port number.
    addr_->sin_port = htons ((uint16_t) atoi (delimiter + 1));
}
