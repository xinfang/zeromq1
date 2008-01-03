/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __PERF_ZEROMQ_HPP_INCLUDED__
#define __PERF_ZEROMQ_HPP_INCLUDED__

#include "../interfaces/i_transport.hpp"

#include "../../zmq/kernel.h"

namespace perf
{

    class zmq_t : public i_transport
    {
    public:
        zmq_t (bool listen, const char *ip_address, unsigned short port,
              bool xmode = false) :
            kernel (listen, ip_address, port, xmode)
        {
        }

        inline ~zmq_t ()
        {
        }

        inline virtual void send (size_t size)
        {
            assert (size <= 65536);

            //  Send the message
            bool ok = kernel.send (buffer, size, NULL);
	    assert (ok);
        }

        inline virtual size_t receive ()
        {
            void *data;
            size_t size;
	    zmq::free_fn *ffn;
            bool ok = kernel.receive (&data, &size, &ffn);
	    assert (ok);
            if (ffn && data)
                ffn (data);
            return size;
        }

    protected:
        zmq::kernel_t kernel;
	unsigned char buffer [65536];
    };

}

#endif
