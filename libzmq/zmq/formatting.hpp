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

#ifndef __ZMQ_ZMQ_FORMATTING_HPP_INCLUDED__
#define __ZMQ_ZMQ_FORMATTING_HPP_INCLUDED__

//  Include this file to define standard POSIX text formatting functions
//  that are missing on Windows platform.

#include <zmq/platform.hpp>

#ifdef ZMQ_HAVE_WINDOWS
#if (_MSC_VER >= 1400)
#define zmq_sprintf sprintf_s
#define zmq_snprintf _snprintf_s
#define zmq_strcat strcat_s
#define zmq_strncpy strncpy_s
#else
#define zmq_sprintf sprintf
#define zmq_snprintf _snprintf
#define zmq_strcat strcat
#define zmq_strncpy strncpy
#endif
#else
#define zmq_sprintf sprintf
#define zmq_snprintf snprintf
#define zmq_strcat strcat
#define zmq_strncpy strncpy
#endif

#endif
