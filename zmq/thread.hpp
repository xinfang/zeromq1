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

#ifndef __ZMQ_THREAD_HPP_INCLUDED__
#define __ZMQ_THREAD_HPP_INCLUDED__

#include "platform.hpp"
#include "declspec_export.hpp"
#ifdef ZMQ_HAVE_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace zmq
{

    typedef void (thread_fn) (void*);

    class declspec_export thread_t
    {
    public:
        thread_t (thread_fn *tfn_, void *arg_);
        ~thread_t ();

    private:

#ifdef ZMQ_HAVE_WINDOWS
        static DWORD WINAPI thread_routine (LPVOID arg_);
        HANDLE descriptor;
#else
        static void *thread_routine (void *arg_);
        pthread_t descriptor;
#endif

        thread_fn *tfn;
        void *arg;
    };

}

#endif
