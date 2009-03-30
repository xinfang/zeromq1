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
#include <zmq.h>

readonly globaldef ZMQ__SCOPE_LOCAL = ZMQ_SCOPE_LOCAL;
readonly globaldef ZMQ__SCOPE_GLOBAL = ZMQ_SCOPE_GLOBAL;

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

unsigned long ZMQ_CREATE (struct dsc$descriptor_s *host_desc_,
    unsigned long *obj_)
{
    char *host;

    lib$establish (lib$sig_to_ret);
    host = dscdup (host_desc_);
    *obj_ = (unsigned long) zmq_create (host);
    free (host);
    return (SS$_NORMAL);
}

unsigned long ZMQ_DESTROY (unsigned long obj_)
{
    lib$establish (lib$sig_to_ret);
    zmq_destroy ((void*) obj_);
    return (SS$_NORMAL);
}

unsigned long ZMQ_CREATE_EXCHANGE (unsigned long obj_,
    struct dsc$descriptor_s *exc_desc_, int scope_,
    struct dsc$descriptor_s *nic_desc_, int style_, int *eid_)
{
    char *exc;
    char *nic;

    lib$establish (lib$sig_to_ret);
    exc = dscdup (exc_desc_);
    nic = dscdup (nic_desc_);
    *eid_ = zmq_create_exchange ((void*) obj_, exc, scope_, nic, style_);
    free (exc);
    free (nic);
    return (SS$_NORMAL);
}

unsigned long ZMQ_CREATE_QUEUE (unsigned long obj_,
    struct dsc$descriptor_s *que_desc_, int scope_,
    struct dsc$descriptor_s *nic_desc_, int64_t hwm_, int64_t lwm_, 
    int64_t swap_, int *qid_)
{
    char *que;
    char *nic;

    lib$establish (lib$sig_to_ret);
    que = dscdup (que_desc_);
    nic = dscdup (nic_desc_);
    *qid_ = zmq_create_queue ((void*) obj_, que, scope_, nic, hwm_, lwm_, swap_);
    return (SS$_NORMAL);
}

unsigned long ZMQ_BIND (unsigned long obj_, struct dsc$descriptor_s *exc_desc_,
    struct dsc$descriptor_s *que_desc_, struct dsc$descriptor_s *exc_options_,
    struct dsc$descriptor_s *que_options_)
{
    char *exc;
    char *que;

    char *exc_options;
    char *que_options;

    lib$establish (lib$sig_to_ret);
    exc = dscdup (exc_desc_);
    que = dscdup (que_desc_);
    exc_options = dscdup (exc_options_);
    que_options = dscdup (que_options_);
    zmq_bind ((void *) obj_, exc, que, exc_options, que_options);
    free (exc);
    free (que);
    free (exc_options);
    free (que_options);
    return (SS$_NORMAL);
}

unsigned long ZMQ_SEND (unsigned long obj_, int eid_, void *data_, 
    uint64_t size_, int block_)
{
    lib$establish (lib$sig_to_ret);
    zmq_send ((void*) obj_, eid_, data_, size_, block_);
    return (SS$_NORMAL);
}

unsigned long ZMQ_RECEIVE (unsigned long obj_, void **data_, uint64_t *size_,
    uint32_t *type_, int block_)
{
    lib$establish (lib$sig_to_ret);
    zmq_receive ((void*) obj_, data_, size_, type_, block_);
    return (SS$_NORMAL);
}
