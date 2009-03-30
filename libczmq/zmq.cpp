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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <zmq.hpp>
#include <zmq.h>

struct context_t
{
    zmq::dispatcher_t *dispatcher;
    zmq::locator_t *locator;    
    zmq::i_thread *io_thread;
    zmq::api_thread_t *api_thread;
};

void *zmq_create (const char *host_)
{   
    //  Create the context.
    context_t *context = new context_t;
    assert (context);
    context->dispatcher = new zmq::dispatcher_t (2);
    assert (context->dispatcher);
    context->locator = new zmq::locator_t (host_);
    assert (context->locator);    
    context->io_thread = zmq::io_thread_t::create (context->dispatcher);
    assert (context->io_thread);
    context->api_thread = zmq::api_thread_t::create (context->dispatcher,
        context->locator);
    assert (context->api_thread);

    return (void*) context;
}

void zmq_destroy (void *object_)
{
    //  Get the context.
    context_t *context = (context_t*) object_;

    //  Deallocate the 0MQ infrastructure.   
    delete context->locator;
    delete context->dispatcher;
    delete context;
}

void zmq_mask (void *object_, uint32_t notifications_)
{
    //  Get the context.
    context_t *context = (context_t*) object_;  

    //  Forward the call.
    context->api_thread->mask (notifications_);
}

int zmq_create_exchange (void *object_, const char *name_, int scope_,
    const char *location_, int style_)
{
    //  Get the context.
    context_t *context = (context_t*) object_;

    //  Forward the call to native 0MQ library.
    return context->api_thread->create_exchange (name_, (zmq::scope_t) scope_,
        location_, context->io_thread, 1, &context->io_thread,
        (zmq::style_t) style_);
}

int zmq_create_queue (void *object_, const char *name_, int scope_,
    const char *location_, int64_t hwm_, int64_t lwm_, int64_t swap_)
{
    //  Get the context.
    context_t *context = (context_t*) object_;

    //  Forward the call to native 0MQ library.
    return context->api_thread->create_queue (name_, (zmq::scope_t) scope_,
        location_, context->io_thread, 1, &context->io_thread,
        hwm_, lwm_, swap_);
}

void zmq_bind (void *object_, const char *exchange_name_,
     const char *queue_name_, const char *exchange_options_,
     const char *queue_options_)
{
    //  Get the context.
    context_t *context = (context_t*) object_;

    //  Forward the call to native 0MQ library.
    context->api_thread->bind (exchange_name_, queue_name_,
        context->io_thread, context->io_thread,
        exchange_options_, queue_options_);
}

int zmq_send (void *object_, int exchange_, void *data_, uint64_t size_,
    int block_)
{
    //  Get the context.
    context_t *context = (context_t*) object_;
    //  Forward the call to native 0MQ library.
    zmq::message_t msg ((size_t) size_);
    memcpy (msg.data (), data_, (size_t) size_);
    return context->api_thread->send (exchange_, msg,
        block_ ? true : false);
}

int zmq_receive (void *object_, void **data_, uint64_t *size_,
    uint32_t *type_, int block_)
{
    //  Get the context.
    context_t *context = (context_t*) object_;

    //  Forward the call to native 0MQ library.
    zmq::message_t msg;
    int qid = context->api_thread->receive (&msg, block_ ? true : false);

    //  Create a buffer and copy the data into it.
    void *buf = malloc (msg.size ());
    assert (buf);
    memcpy (buf, msg.data (), msg.size ());

    assert (data_);
    *data_ = buf;
    if (size_)
        *size_ = msg.size ();
    if (type_)
        *type_ = msg.type ();

    return qid;
}

void zmq_free (void *data_)
{
    free (data_);
}
