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

#ifndef __PERF_ECHO_HPP_INCLUDED__
#define __PERF_ECHO_HPP_INCLUDED__

#include "../interfaces/i_worker.hpp"

namespace perf
{

    class echo_t : public i_worker
    {
    public:
        inline echo_t (int message_count_, size_t message_size_) :
            message_count (message_count_), message_size (message_size_)
        {
        }

        inline virtual void run (i_transport &transport_, const char* = NULL,
            unsigned int thread_id_ = 0)
        {
            for (int message_nbr = 0; message_nbr != message_count;
                message_nbr++){
                size_t size = transport_.receive (thread_id_);
                assert (size == message_size);
                transport_.send (size, thread_id_);
            }
        }
    protected:
        int message_count;
        size_t message_size;
    };

}

#endif
