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

using namespace System;
using namespace System::Runtime::InteropServices;

#pragma managed

struct Dnzmq_context_t
{
    zmq::locator_t *locator;
    zmq::dispatcher_t *dispatcher;
    zmq::i_thread *io_thread;
    zmq::api_thread_t *api_thread;
};

public ref class Dnzmq
{
public:

    Dnzmq (String^ host_);
    ~Dnzmq ();

    void mask (int message_mask_);
    int create_exchange (String^ exchange_, int scope_, String^ nic_, 
        int style_);
    int create_queue (String^ queue_, int scope_, String^ nic_, int64_t hwm_,
        int64_t lwm_, int64_t swap_size_);
    void bind (String^ exchange_, String^ queue_, 
        String^ exchange_arguments_, String^ queue_arguments_);
    void send (int eid_, array<Byte>^ data_);
    array<Byte>^ receive ();
       
    static const int SCOPE_LOCAL = 0;
    static const int SCOPE_GLOBAL = 1;
    
    static const int MESSAGE_DATA = 1;
    static const int MESSAGE_GAP = 2;

    static const int STYLE_DATA_DISTRIBUTION = 1;
    static const int STYLE_LOAD_BALANCING = 2;

private:

    Dnzmq_context_t *context; 
};

Dnzmq::Dnzmq(String^ host_)
{
    //  Convert input arguments to char*.
    char* host = (char*)(void*) Marshal::StringToHGlobalAnsi(host_);
   
    //  Call WSAStartUp.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 ||
        HIBYTE (wsa_data.wVersion) == 2);

    //  Create the context.
    context = new Dnzmq_context_t;
    assert (context);
    context->locator = new zmq::locator_t (host);
    assert (context->locator);
    context->dispatcher = new zmq::dispatcher_t (2);
    assert (context->dispatcher);
    context->io_thread = zmq::select_thread_t::create (context->dispatcher);
    assert (context->io_thread);
    context->api_thread = zmq::api_thread_t::create (context->dispatcher,
        context->locator);
    assert (context->api_thread);

    Marshal::FreeHGlobal ((IntPtr) host);
}
   
Dnzmq::~Dnzmq ()
{
    //  Deallocate the 0MQ infrastructure.
    delete context->dispatcher;
    delete context->locator;
    delete context;
}

void Dnzmq::mask (int message_mask_)
{
    //  Set notifications for the transport.
    context->api_thread->mask (message_mask_);
}

int Dnzmq::create_exchange (String^ exchange_, int scope_, String^ nic_,
                            int style_)
{   
    //  Convert input parameters to char*.
    char* exchange = (char*)(void*) Marshal::StringToHGlobalAnsi (exchange_);
    char* nic = (char*)(void*) Marshal::StringToHGlobalAnsi (nic_);
    
    //  Get the scope.
    zmq::scope_t scope = zmq::scope_local;
    if (scope_ == SCOPE_GLOBAL)
        scope = zmq::scope_global;
    
    //  Get the style.
    zmq::style_t style = zmq::style_load_balancing;
    if (style_ == STYLE_DATA_DISTRIBUTION)
        style = zmq::style_data_distribution;
    
    //  Forward the call to native 0MQ library.
    int eid = context->api_thread->create_exchange (exchange, 
        scope, nic, context->io_thread, 1, &context->io_thread);

    //  Deallocate the parameters.
    Marshal::FreeHGlobal ((IntPtr) exchange);
    Marshal::FreeHGlobal ((IntPtr) nic);

    return eid;
}

int Dnzmq::create_queue (String^ queue_, int scope_, String^ nic_, 
        int64_t hwm_, int64_t lwm_, int64_t swap_size_)
{      
    //  Convert input parameters to char*.
    char* queue = (char*)(void*) Marshal::StringToHGlobalAnsi (queue_);
    char* nic = (char*)(void*) Marshal::StringToHGlobalAnsi (nic_);
    
    //  Get the scope.
    zmq::scope_t scope = zmq::scope_local;
    if (scope_ == SCOPE_GLOBAL)
        scope = zmq::scope_global;

    //  Forward the call to native 0MQ library.
    int qid = context->api_thread->create_queue (queue, 
        scope, nic, context->io_thread, 1, &context->io_thread, hwm_, lwm_, swap_size_);

    Marshal::FreeHGlobal ((IntPtr) queue);
    Marshal::FreeHGlobal ((IntPtr) nic);

    return qid;
}

void Dnzmq::bind (String^ exchange_, String^ queue_, 
    String^ exchange_arguments_, String^ queue_arguments_)
{
    //  Convert input parameters to char*.
    char* exchange = (char*)(void*) Marshal::StringToHGlobalAnsi (exchange_);
    char* queue = (char*)(void*) Marshal::StringToHGlobalAnsi (queue_);
    char* exchange_arguments = (char*)(void*) Marshal::StringToHGlobalAnsi (exchange_arguments_);
    char* queue_arguments = (char*)(void*) Marshal::StringToHGlobalAnsi (queue_arguments_);
            
    //  Forward the call to native 0MQ library.
   context->api_thread->bind (exchange, queue, context->io_thread,
        context->io_thread, exchange_arguments, queue_arguments);
   
    Marshal::FreeHGlobal ((IntPtr) exchange);        
    Marshal::FreeHGlobal ((IntPtr) queue);
    Marshal::FreeHGlobal ((IntPtr) exchange_arguments);
    Marshal::FreeHGlobal ((IntPtr) queue_arguments);
}

void Dnzmq::send (int eid_, array<Byte>^ data_)
{
    //  Pin the data to the fixed memory location so that it doesn't
    //  get moved during the call to memcpy.
    pin_ptr<Byte> tmp_data = &data_ [0];

    //  Copy data to the unmanaged buffer.
    void *data = malloc (data_->Length);
    assert (data);
    memcpy (data, (Byte*) tmp_data, data_->Length);
   
    //  Forward the call to native 0MQ library.
    zmq::message_t msg (data, data_->Length, free);
    context->api_thread->send (eid_, msg);
}

array<Byte>^ Dnzmq::receive ()
{
    //  Forward the call to native 0MQ library.
    zmq::message_t msg ;
    context->api_thread->receive (&msg);

    //  Allocate a managed array and pin it down to the memory.
    array<Byte>^ data = gcnew array<Byte> (msg.size ());
    pin_ptr<Byte>  tmp_data = &data [0];

    //  Copy the message to the managed buffer.
    memcpy ((Byte*) tmp_data, msg.data (), msg.size ());

    return data;
}

