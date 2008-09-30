/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "thread.hpp"

namespace zmq
{
    
zmq::thread_t::thread_t (thread_fn *tfn_, void *arg_) :
        tfn (tfn_),
        arg (arg_)
{

#ifdef ZMQ_HAVE_WINXP

    thread_id = CreateThread(NULL, 0, &zmq::thread_t::thread_routine, this, 0, NULL); 
    errno_assert(thread_id);
   
#else

    ret_val = pthread_create(thread_id, NULL, thread_routine, this);
    errno_assert(ret_val!=0);

#endif
}

zmq::thread_t::~thread_t ()
{
#ifdef ZMQ_HAVE_WINXP

    DWORD dwWait;
    dwWait = WaitForSingleObjectEx(thread_id, INFINITE, TRUE);
    thread_assert(dwWait);

#else
    pthread_join (thread_id, NULL);
#endif

}

#ifdef ZMQ_HAVE_WINXP
DWORD WINAPI zmq::thread_t::thread_routine (LPVOID arg_)
{
    thread_t *self = (thread_t*) arg_;
    self->tfn (self->arg);
    return 0;
}
#else
void zmq::thread_t::*thread_routine (void *arg_)
{
    thread_t *self = (thread_t*) arg_;   
    self->tfn (self->arg);
    return NULL;
}
#endif

}






