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

#ifndef __ZMQ_ERR_HPP_INCLUDED__
#define __ZMQ_ERR_HPP_INCLUDED__

#include "platform.hpp"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef ZMQ_HAVE_WINDOWS
#include <Winsock2.h>
#else
#include <netdb.h>
#endif

#ifdef ZMQ_HAVE_WINDOWS

namespace zmq
{

    const char * wsa_error ();

}

#endif



//  Provides convenient way to check for errno-style errors.
#define errno_assert(x) if (!(x)) {\
    perror (NULL);\
    printf ("%s (%s:%d)\n", #x, __FILE__, __LINE__);\
    abort ();\
}

// Provides convenient way to check for errors from getaddrinfo.
#define gai_assert(x) if (x) {\
    const char *errstr = gai_strerror (x);\
    printf ("%s (%s:%d)\n", errstr, __FILE__, __LINE__);\
    abort ();\
}

//  Provides convenient way to check WSA-style errors on Windows.
#define wsa_assert(x) if (!(x)){\
    const char *errstr = wsa_error ();\
    printf ("%s (%s:%d)\n", errstr, __FILE__, __LINE__);\
    abort ();\
}
#define win_assert(x) if (!(x)) {\
    const char *errstr;\
    DWORD last_error = GetLastError (); \
    FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, \
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)errstr, 0, NULL );\
    printf ("%s (%s:%d)\n", errstr, __FILE__, __LINE__);\
    abort ();\
}
#endif
