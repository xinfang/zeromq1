#ifndef __ZMQ_IP_HPP_INCLUDED__
#define __ZMQ_IP_HPP_INCLUDED__

#include <netinet/in.h>

namespace zmq
{
    int resolve_iface (char const* name, struct sockaddr_in* addr);

    //  This function resolves a string in <host-name>:<port-number> format.
    //  If port number is not specified, default_port_ is used instead. If
    //  IP address is not specified (e.g. ":80"), default address is used
    //  instead.
    int resolve_ip_address (sockaddr_in *address_, const char *host_,
        const char *default_address_, const char* default_port_);
};

#endif 
