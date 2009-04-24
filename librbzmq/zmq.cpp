#include <ruby.h>
#include <zmq.hpp>
#include <zmq/err.hpp>

static VALUE rb_zmq;

struct context_t
{
    zmq::dispatcher_t *dispatcher;
    zmq::locator_t *locator;    
    zmq::i_thread *io_thread;
    zmq::api_thread_t *api_thread;
};

static void rb_free(void * p) 
{
	
	//  Get the context.
	context_t *context = (context_t*) p;

	//  Deallocate the 0MQ infrastructure.   
    delete context->locator;
    delete context->dispatcher;
    delete context;
}

static VALUE rb_alloc (VALUE data_)
{
	context_t *context;
	VALUE obj;

	//  Create the context.
    context = new context_t;
    
	//  Create the context.	
	obj = Data_Wrap_Struct (data_, 0, rb_free, context);
	return obj;
}

static VALUE rb_init (VALUE self_, VALUE host_)
{
	//  Get the context.
	context_t *context;
	Data_Get_Struct (self_, context_t, context);
	
	
    //zmq_assert (context);
	assert (context);
    context->dispatcher = new zmq::dispatcher_t (2);
    //zmq_assert (context->dispatcher);
	assert (context->dispatcher);
    context->locator = new zmq::locator_t (STR2CSTR (host_));
    //zmq_assert (context->locator);    
	assert (context->locator);    
    context->io_thread = zmq::io_thread_t::create (context->dispatcher);
    //zmq_assert (context->io_thread);
	assert (context->io_thread);
    context->api_thread = zmq::api_thread_t::create (context->dispatcher,
        context->locator);
    //zmq_assert (context->api_thread);
	assert (context->api_thread);

    return self_;

}

static VALUE rb_mask (VALUE self_, VALUE notifications_)
{
	//  Get the context.
	context_t *context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call.
    context->api_thread->mask (NUM2UINT(notifications_));

	return self_;
}

static VALUE rb_create_exchange (VALUE self_, VALUE name_, VALUE scope_, 
	VALUE location_, VALUE style_)
{
	//  Get the context.
	context_t *context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    return context->api_thread->create_exchange (STR2CSTR (name_), 
		(zmq::scope_t) NUM2INT (scope_), STR2CSTR (location_), 
		context->io_thread, 1, &context->io_thread, 
		(zmq::style_t) NUM2INT (style_));
}

static VALUE rb_create_queue (VALUE self_, VALUE name_, VALUE scope_, 
	VALUE location_, VALUE hwm_, VALUE lwm_, VALUE swap_)
{
	//  Get the context.
	context_t *context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    return context->api_thread->create_queue (STR2CSTR (name_), 
		(zmq::scope_t) NUM2INT (scope_), STR2CSTR (location_), 
		context->io_thread, 1, &context->io_thread, 
		NUM2LL (hwm_), NUM2LL (lwm_), NUM2LL (swap_));
}	

static VALUE rb_bind (VALUE self_, VALUE exchange_name_, VALUE queue_name_,
	VALUE exchange_options_, VALUE queue_options_)
{
	//  Get the context.
	context_t *context;
	Data_Get_Struct (self_, context_t, context);
	
	//  Forward the call to native 0MQ library.
    context->api_thread->bind (STR2CSTR (exchange_name_), 
		STR2CSTR (queue_name_),	context->io_thread, context->io_thread,
		STR2CSTR (exchange_options_), STR2CSTR (queue_options_));
		
	return self_;	
}

static VALUE rb_send (VALUE self_, VALUE exchange_, VALUE data_, VALUE size_,
	VALUE block_)
{
	//  Get the context.
	context_t *context;
	Data_Get_Struct (self_, context_t, context);

	//  Forward the call to native 0MQ library.
    zmq::message_t msg ((size_t) size_);
    memcpy (msg.data (), (void*) STR2CSTR (data_), (size_t) size_);
    return context->api_thread->send (NUM2INT (exchange_), msg,
        block_ ? true : false);
}

static VALUE rb_receive (VALUE self_, VALUE block_)
{
	//  Get the context.
	context_t *context;
	Data_Get_Struct (self_, context_t, context);


	//  Forward the call to native 0MQ library.
    zmq::message_t msg;
    int qid = context->api_thread->receive (&msg, 
    	NUM2INT (block_ ? true : false));

    //  Create a buffer and copy the data into it.
    void *buf = malloc (msg.size ());
    //zmq_assert (buf);
	assert (buf);
    memcpy (buf, msg.data (), msg.size ());

    return rb_str_new2 ((char *) buf), ULL2NUM (msg.size ()), 
    	UINT2NUM (msg.type ()), INT2NUM (qid);
}

extern "C" {

void Init_zmq() {
	
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
	rb_define_method (rb_zmq, "receive", (VALUE(*)(...)) rb_receive, 4);
	rb_define_method (rb_zmq, "free", (VALUE(*)(...)) rb_free, 0); 		

}
}
