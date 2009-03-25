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

/*
    This file contains a simple set of wrapper functions intended to make
    0MQ more easily usable on OpenVMS with languages other than C/C++.

    The associated build procedure links these wrappers into a shareable
    image that can in turn be linked with application code.

    BRC 17-Mar-2009
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <descrip.h>
#include <ctype.h>
#include <zmq/czmq.h>

readonly globaldef ZMQ__SCOPE_LOCAL = CZMQ_SCOPE_LOCAL;
readonly globaldef ZMQ__SCOPE_GLOBAL = CZMQ_SCOPE_GLOBAL;

static char *rtrim (char *str)
{
    char *cp;

    cp = &str[strlen (str) - 1];
    while (isspace (*cp)) {
        *cp = '\0';
        cp--;
    }
    return (str);
}

static char *dscdup (struct dsc$descriptor_s *dsc)
{
    char *tmp;

    if (!dsc)
        return (NULL);
    tmp = (char*) malloc ((dsc->dsc$w_length + 1) * sizeof (char));
    if (!tmp)
        return (NULL);

    memcpy (tmp, dsc->dsc$a_pointer, dsc->dsc$w_length);
    tmp [dsc->dsc$w_length] = '\0';
    return (rtrim (tmp));
}

unsigned long ZMQ_CREATE (struct dsc$descriptor_s *host_desc,
    unsigned long *obj)
{
    char *host;

    lib$establish (lib$sig_to_ret);
    host = dscdup (host_desc);
    *obj = (unsigned long) czmq_create (host);
    free (host);
    return (SS$_NORMAL);
}

unsigned long ZMQ_DESTROY (unsigned long obj)
{
    lib$establish (lib$sig_to_ret);
    czmq_destroy ((void*) obj);
    return (SS$_NORMAL);
}

unsigned long ZMQ_CREATE_EXCHANGE (unsigned long obj,
    struct dsc$descriptor_s *exc_desc, int scope,
    struct dsc$descriptor_s *nic_desc, int *eid)
{
    char *exc;
    char *nic;

    lib$establish (lib$sig_to_ret);
    exc = dscdup (exc_desc);
    nic = dscdup (nic_desc);
    *eid = czmq_create_exchange ((void*) obj, exc, scope, nic);
    free (exc);
    free (nic);
    return (SS$_NORMAL);
}

unsigned long ZMQ_CREATE_QUEUE (unsigned long obj,
    struct dsc$descriptor_s *que_desc, int scope,
    struct dsc$descriptor_s *nic_desc)
{
    char *que;
    char *nic;

    lib$establish (lib$sig_to_ret);
    que = dscdup (que_desc);
    nic = dscdup (nic_desc);
    czmq_create_queue ((void*) obj, que, scope, nic);
    return (SS$_NORMAL);
}

unsigned long ZMQ_BIND (unsigned long obj, struct dsc$descriptor_s *exc_desc,
    struct dsc$descriptor_s *que_desc)
{
    char *exc;
    char *que;

    lib$establish (lib$sig_to_ret);
    exc = dscdup (exc_desc);
    que = dscdup (que_desc);
    czmq_bind ((void *) obj, exc, que);
    free (exc);
    free (que);
    return (SS$_NORMAL);
}

unsigned long ZMQ_SEND (unsigned long obj, int eid, void *data, size_t dlen,
    void *ffn)
{
    lib$establish (lib$sig_to_ret);
    czmq_send ((void*) obj, eid, data, dlen, (czmq_free_fn *) ffn);
    return (SS$_NORMAL);
}

unsigned long ZMQ_RECEIVE (unsigned long obj, void **data, size_t *dlen,
    void **ffn)
{
    lib$establish (lib$sig_to_ret);
    czmq_receive ((void*) obj, data, dlen, (czmq_free_fn **) ffn);
    return (SS$_NORMAL);
}
