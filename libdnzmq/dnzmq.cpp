// This is the main DLL file.
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

#include <zmq.hpp>

using namespace System;
using namespace System::Runtime::InteropServices;

#pragma unmanaged

void unmanaged_free (void *p_)
{
    free (p_);
}
void *unmanaged_malloc (int size_)
{
    return malloc (size_);
}

#pragma managed

namespace zmq
{

    #define ZMQ_SCOPE_LOCAL 0
    #define ZMQ_SCOPE_GLOBAL 1

    struct context_t
    {
        zmq::locator_t *locator;
        zmq::dispatcher_t *dispatcher;
        zmq::i_thread *io_thread;
        zmq::api_thread_t *api_thread;
    };

    public __gc class Dnzmq
    {
        public:
     
            void create (const char *host_);
            void destroy ();
            int create_exchange (const char * exchange_, int scope_,
                const char *nic_);
            int create_queue (const char *queue_, int scope_,
                const char *nic_);
            void bind (const char *exchange_, const char *queue_);
            void send (int eid_, void *data_, size_t size_);
            void receive (void **data, size_t *size_);

        private:
            context_t *context; 
    };
    
    void Dnzmq::create (const char *host_)
    {
        //  Call WSAStartUp.
        WORD version_requested = MAKEWORD (2, 2);
        WSADATA wsa_data;
        int rc = WSAStartup (version_requested, &wsa_data);
        assert (rc == 0);
        assert (LOBYTE (wsa_data.wVersion) == 2 ||
            HIBYTE (wsa_data.wVersion) == 2);

        //  Create the context.
        context = new context_t;
        assert (context);
        context->locator = new zmq::locator_t (host_);
        assert (context->locator);
        context->dispatcher = new zmq::dispatcher_t (2);
        assert (context->dispatcher);
        context->io_thread = zmq::select_thread_t::create (context->dispatcher);
        assert (context->io_thread);
        context->api_thread = zmq::api_thread_t::create (context->dispatcher,
            context->locator);
        assert (context->api_thread);
    }

    void Dnzmq::destroy ()
    {
        //  Deallocate the 0MQ infrastructure.
        delete context->dispatcher;
        delete context->locator;
        delete context;
    }

    int Dnzmq::create_exchange (const char *exchange_, int scope_,
        const char *nic_)
    {   
        //  Get the scope.
        zmq::scope_t scope = zmq::scope_local;
        if (scope_ == ZMQ_SCOPE_GLOBAL)
            scope = zmq::scope_global;

        //  Forward the call to native 0MQ library.
        return context->api_thread->create_exchange (exchange_, 
            scope, nic_, context->io_thread, 1, &context->io_thread);
    }

    int Dnzmq::create_queue (const char *queue_, int scope_, const char *nic_)
    {          
        //  Get the scope.
        zmq::scope_t scope = zmq::scope_local;
        if (scope_ == ZMQ_SCOPE_GLOBAL)
            scope = zmq::scope_global;

        //  Forward the call to native 0MQ library.
       return context->api_thread->create_queue (queue_, 
           scope, nic_, context->io_thread, 1, &context->io_thread);
    }

    void Dnzmq::bind (const char * exchange_, const char * queue_)
    {
        //  Forward the call to native 0MQ library.
        context->api_thread->bind (exchange_, queue_, context->io_thread,
            context->io_thread);
   
    }

    void Dnzmq::send (int eid_, void * data_, size_t size_)
    {
        //  Copy data to the unmanaged buffer.
        void *data = unmanaged_malloc (size_);
        assert (data);
        memcpy (data, data_, size_);

        //  Forward the call to native 0MQ library.
        zmq::message_t msg (data, size_, unmanaged_free);
        context->api_thread->send (eid_, msg);
    }

    void Dnzmq::receive (void **data, size_t *size_)
    {
        //  Forward the call to native 0MQ library.
        zmq::message_t msg ;
        context->api_thread->receive (&msg);

        //  Copy the data to the managed buffer.
        void *buf = malloc (msg.size ());
        assert (buf);
        memcpy (buf, msg.data (), msg.size ());
  
        //  Return the message.
        *data = buf;
        *size_ = msg.size ();
    }

}