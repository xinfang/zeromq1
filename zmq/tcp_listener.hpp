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

#ifndef __ZMQ_TCP_LISTENER_HPP_INCLUDED__
#define __ZMQ_TCP_LISTENER_HPP_INCLUDED__

#include "stdint.hpp"

namespace zmq
{

    //  The class encapsulating simple TCP listening socket.

    class tcp_listener_t
    {
    public:

        tcp_listener_t (const char *interface_, uint16_t port_);

        inline int get_fd ()
        {
            return s;
        }

        int accept ();

    private:

        //  Underlying socket
        int s;
    };

}

#endif
