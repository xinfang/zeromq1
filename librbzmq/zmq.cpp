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

#include <zmq.hpp>
#include <zmq/err.hpp>
#include <ruby.h>

struct context_t
{
    zmq::dispatcher_t *dispatcher;
    zmq::locator_t *locator;    
    zmq::i_thread *io_thread;
    zmq::api_thread_t *api_thread;
};

static VALUE rb_zmq;
static VALUE rb_data;

static void rb_free (void *p)
{
	context_t *context = (context_t*) p;
	
	//  Deallocate the 0MQ infrastructure.   
    delete context->locator;
    delete context->dispatcher;
    delete context;	
}

static VALUE rb_alloc (VALUE self_)
{
	context_t *context;
	VALUE obj;
	
	context = new context_t;
	obj = Data_Wrap_Struct (self_, 0, rb_free, context);
	
	return obj;
}

static VALUE rb_init (VALUE self_, VALUE host_)
{
	//  Get the context.
	context_t* context;
	Data_Get_Struct (self_, context_t, context);
	
	context->dispatcher = new zmq::dispatcher_t (2);
    zmq_assert (context->dispatcher);

    context->locator = new zmq::locator_t (StringValueCStr (host_));
    zmq_assert (context->locator);    
	
    context->io_thread = zmq::io_thread_t::create (context->dispatcher);
    zmq_assert (context->io_thread);
	
    context->api_thread = zmq::api_thread_t::create (context->dispatcher,
        context->locator);
    zmq_assert (context->api_thread);

    return self_;
}

static VALUE rb_mask (VALUE self_, VALUE notifications_)
{
	//  Get the context.
	context_t* context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call.
    context->api_thread->mask (NUM2UINT (notifications_));

	return self_;
}

static VALUE rb_create_exchange (VALUE self_, VALUE name_, VALUE scope_, 
	VALUE location_, VALUE style_)
{
	//  Get the context.
	context_t* context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    return INT2NUM (context->api_thread->create_exchange (
    	StringValueCStr (name_), (zmq::scope_t) NUM2INT (scope_), 
    	StringValueCStr (location_), context->io_thread, 1, &context->io_thread, 
		(zmq::style_t) NUM2INT (style_)));
}

static VALUE rb_create_queue (VALUE self_, VALUE name_, VALUE scope_, 
	VALUE location_, VALUE hwm_, VALUE lwm_, VALUE swap_)
{
	//  Get the context.
	context_t* context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    return INT2NUM ((context->api_thread->create_queue (
    	StringValueCStr (name_), (zmq::scope_t) NUM2INT (scope_), 
    	StringValueCStr (location_), context->io_thread, 1, &context->io_thread, 
		NUM2LL (hwm_), NUM2LL (lwm_), NUM2LL (swap_))));
}	

static VALUE rb_bind (VALUE self_, VALUE exchange_name_, VALUE queue_name_,
	VALUE exchange_options_, VALUE queue_options_)
{
	//  Get the context.
	context_t* context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    context->api_thread->bind (StringValueCStr (exchange_name_), 
		StringValueCStr (queue_name_),	context->io_thread, context->io_thread,
		StringValueCStr (exchange_options_), StringValueCStr (queue_options_));
		
	return self_;	
}

static VALUE rb_send (VALUE self_, VALUE exchange_, VALUE data_, VALUE size_,
	VALUE block_)
{
	//  Get the context.
	context_t* context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    zmq::message_t msg ((size_t) NUM2ULL (size_));
    memcpy (msg.data (), (void*) StringValueCStr (data_), 
    	(size_t) NUM2ULL (size_));
    return context->api_thread->send (NUM2INT (exchange_), msg,
        NUM2INT (block_) ? true : false);
}

static VALUE rb_receive (VALUE self_, VALUE block_)
{
	//  Get the context.
	context_t* context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    zmq::message_t msg;
    int qid = context->api_thread->receive (&msg, 
    	NUM2INT(block_) ? true : false);
    
    VALUE rb_msg = rb_str_new ((char *) msg.data (), msg.size ());
	VALUE rb_type = INT2NUM (msg.type ());
	VALUE rb_qid = INT2NUM (qid);
    
	return rb_struct_new (rb_data, rb_msg, rb_type, rb_qid, NULL);
}

void Init_librbzmq() {
	
	rb_zmq = rb_define_class ("Zmq", rb_cObject);
	
	rb_define_alloc_func (rb_zmq, rb_alloc);
	
	rb_define_method (rb_zmq, "initialize", (VALUE(*)(...)) rb_init, 1);
	rb_define_method (rb_zmq, "mask", (VALUE(*)(...)) rb_mask, 1);
	rb_define_method (rb_zmq, "create_exchange", 
		(VALUE(*)(...)) rb_create_exchange, 4);
	rb_define_method (rb_zmq, "create_queue", 
		(VALUE(*)(...)) rb_create_queue, 6);
	rb_define_method (rb_zmq, "bind", (VALUE(*)(...)) rb_bind, 4);
	rb_define_method (rb_zmq, "send", (VALUE(*)(...)) rb_send, 4);
	rb_define_method (rb_zmq, "receive", (VALUE(*)(...)) rb_receive, 1);
	rb_define_method (rb_zmq, "free", (VALUE(*)(...)) rb_free, 0);
	
	rb_data = rb_struct_define (NULL, "msg", "type", "qid", NULL);
	rb_define_const (rb_zmq, "DATA", rb_data);

	rb_define_global_const ("ZMQ_SCOPE_LOCAL", INT2NUM (zmq::scope_local));
	rb_define_global_const ("ZMQ_SCOPE_PROCESS", INT2NUM (zmq::scope_process));
	rb_define_global_const ("ZMQ_SCOPE_GLOBAL", INT2NUM (zmq::scope_global));
	rb_define_global_const ("ZMQ_MESSAGE_DATA", INT2NUM (zmq::message_data));
	rb_define_global_const ("ZMQ_MESSAGE_GAP", INT2NUM (zmq::message_gap));
	rb_define_global_const ("ZMQ_STYLE_DATA_DISTRIBUTION", 
		INT2NUM (zmq::style_data_distribution));
	rb_define_global_const ("ZMQ_STYLE_LOAD_BALANCING", 
		INT2NUM (zmq::style_load_balancing));
	rb_define_global_const ("ZMQ_NO_LIMIT", INT2NUM (zmq::no_limit));
	rb_define_global_const ("ZMQ_NO_SWAP", INT2NUM (zmq::no_swap));
	rb_define_global_const ("ZMQ_TRUE", INT2NUM (1));
	rb_define_global_const ("ZMQ_FALSE", INT2NUM (0));
}
