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

#include <stddef.h>
#include <zmq/export.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#define CZMQ_SCOPE_LOCAL 0
#define CZMQ_SCOPE_GLOBAL 1

typedef void (czmq_free_fn) (void *data_);

void ZMQ_EXPORT *czmq_create (const char *host_);
void ZMQ_EXPORT czmq_destroy (void *obj_);
int ZMQ_EXPORT czmq_create_exchange (void *obj_, const char *exchange_, int scope_,
    const char *nic_);
int ZMQ_EXPORT czmq_create_queue (void *obj_, const char *queue_, int scope_,
    const char *nic_);
void ZMQ_EXPORT czmq_bind (void *obj_, const char *exchange_, const char *queue_);
void ZMQ_EXPORT czmq_send (void *obj_, int eid_, void *data_, size_t size,
    czmq_free_fn *ffn_);
void ZMQ_EXPORT czmq_receive (void *obj_, void **data_, size_t *size_,
    czmq_free_fn **ffn_);

#ifdef __cplusplus
}
#endif
