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

/* the platform we are building */
#define @ZMQ_PLATFORM_NAME@ 1

/* windows specific */
#ifdef ZMQ_HAVE_WINDOWS
#define _WINSOCKAPI_
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#define ZMQ_EXPORT __declspec(dllexport)
#endif

/* else ? */
#ifndef ZMQ_HAVE_WINDOWS
#define ZMQ_EXPORT
#endif

/* Have Stream Control Transmission Protocol */
#cmakedefine ZMQ_HAVE_SCTP 1
