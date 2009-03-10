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
#include <stdio.h>

#include <zmq/dummy_locator.hpp>
#include <zmq/formatting.hpp>

zmq::dummy_locator_t::dummy_locator_t ()
{
}

void zmq::dummy_locator_t::register_endpoint (const char *name_,
    const char *location_)
{
}
 
void zmq::dummy_locator_t::resolve_endpoint (const char *name_,
    char *location_, size_t location_size_)
{
    assert (location_size_ >= 1);
    zmq_strncpy (location_, name_, location_size_);
    location_ [location_size_ - 1] = 0;
}
