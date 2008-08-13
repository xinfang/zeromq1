#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>

#include <assert.h>
#include <string.h>

#include <string>

#include "ip.hpp"
#include "config.h"

# if ZMQ_HAVE_SOLARIS

#include <sys/sockio.h>
#include <net/if.h>

int zmq::resolve_iface (char const* name, struct sockaddr_in* addr)
{
    int rc = -1;
    int fd;
    
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) {
        assert (0);
        return -1;
    }
    
    struct lifnum ifn;
    ifn.lifn_family = AF_UNSPEC;
    ifn.lifn_flags = 0;
    if (ioctl (fd, SIOCGLIFNUM, (char*)&ifn) < 0) {
        assert (0);
        close (fd);
        /* could not determine number of interfaces */
        return -1;
    }

    char* ifr;
    size_t ifr_size = sizeof (struct lifreq) * ifn.lifn_count;
    
    if ((ifr = malloc (ifr_size)) == NULL) {
        assert (0);
        close (fd);
        return -1;
    }
    
    struct lifconf ifc;
    ifc.lifc_family = AF_UNSPEC;
    ifc.lifc_flags = 0;
    ifc.lifc_len = ifr_size;
    ifc.lifc_buf = ifr;
    if (ioctl (fd, SIOCGLIFCONF, (char*)&ifc) < 0) {
        assert (0);
        close (fd);
        free (ifr);
        return -1;
    }

    struct lifreq* ifrp;
    int n;

    ifrp = ifc.lifc_req;
    for (n = 0 ; n < ifc.lifc_len / sizeof (struct lifreq) ; ++n, ifrp++) {
        if (!strcmp (name, ifrp->lifr_name)) {
            if (ioctl (fd, SIOCGLIFADDR, (char*)ifrp) == 0) {
                if (ifrp->lifr_addr.ss_family == AF_INET) {
                    memcpy (addr, (void*)&ifrp->lifr_addr, sizeof (struct sockaddr_in));
                    rc = 0;
                }
            }
        }
    }

    close (fd);

    return rc;
}
# else
# include <ifaddrs.h>

int zmq::resolve_iface (char const* name, struct sockaddr_in* addr)
{
    int rc = -1;
    struct ifaddrs* ifa = NULL;

    assert (name != NULL);
    assert (addr != NULL);

    memset (addr, 0, sizeof (struct sockaddr_in));
    addr->sin_family = AF_UNSPEC;
    addr->sin_addr.s_addr = INADDR_NONE;

# if HAVE_GETIFADDRS
    if (getifaddrs (&ifa) != 0) 
        return -1;
    
    assert (ifa != NULL);

    for (struct ifaddrs* ifp = ifa ; ifp != NULL ; ifp = ifp->ifa_next) {
        if (ifp->ifa_addr 
            && ifp->ifa_addr->sa_family == AF_INET 
            && !strcmp (name, ifp->ifa_name)) 
        {
            addr->sin_family = AF_INET;
            addr->sin_addr = 
                ((struct sockaddr_in*)ifp->ifa_addr)->sin_addr;
            rc = 0;
        }
    }
    
    freeifaddrs (ifa);
# endif 
    return rc;
}
# endif

int zmq::resolve_ip_address (sockaddr_in *address_, const char *host_,
    const char *default_address_, const char* default_port_)
{
    const char *port;
    const char *address;
    std::string host;

    if (!host_) {

        //  If host is not given, use default values.
        // assert (default_port_);
        assert (default_address_);
        port = default_port_;
        address = default_address_;
    }
    else {

        //  Delimit the IP address portion and port portion of the host string.
        host = host_;
        std::string::size_type pos = host.find (':');

        //  Determine port to use.
        if (pos == std::string::npos) {
            // assert (default_port_);
            port = default_port_;
        }
        else
            port = host.c_str () + pos + 1;

        //  Determine IP address to use.
        if (pos == 0) {
            assert (default_address_);
            address = default_address_;
        }
        else {
            host [pos] = 0;
            address = host.c_str ();
        }
    }

    //  Resolve the host name and service name.
    struct addrinfo req;
    memset (&req, 0, sizeof req);
    struct addrinfo *res;
    req.ai_family = AF_INET;
    int rc = getaddrinfo (address, NULL, &req, &res);
    if (rc == 0) {
        *address_ = *((sockaddr_in *) res->ai_addr);
        freeaddrinfo (res);
    } else {
        rc = resolve_iface (host.c_str(), address_);
    }
    if (!port)
        address_->sin_port = 0;
    else
        address_->sin_port = htons (atoi (port));
    if (rc < 0) {
        fprintf (stderr, "failed to resolve '%s'\n",
                 host.c_str());
    }
    assert (rc == 0); // until zmq_server is fixd
    return rc;
}
