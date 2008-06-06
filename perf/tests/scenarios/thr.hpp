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

#ifndef __PERF_THR_HPP_INCLUDED__
#define __PERF_THR_HPP_INCLUDED__

#include <vector>

#include "../../transports/i_transport.hpp"
#include "../../helpers/time.hpp"

namespace perf
{
    struct thr_worker_args_t
    {
        int id;
        int msg_size;
        int roundtrip_count;
        const char *listen_ip;
        unsigned short listen_port_base;
        const char *locator_ip;
        unsigned short locator_port;

        time_instant_t start_time;
        time_instant_t stop_time;
    };


    std::vector<time_instant_t> local_thr (i_transport *transport_, 
        size_t msg_size_, int roundtrip_count_)
    {

        std::vector<time_instant_t> start_stop_time (2, 0);

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++)
        {
            
            size_t size = transport_->receive ();
            if (msg_nbr == 0)
                start_stop_time [0] = now ();
            
            // check incomming message size
            assert (size == msg_size_);
        }

        start_stop_time [1] = now(); 

        // send sync message
        transport_->send (1);

        return start_stop_time;
    }


    void remote_thr (i_transport *transport_, size_t msg_size_, int roundtrip_count_)
    {
        
        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++)
        {
            transport_->send (msg_size_);
        }

        // wait for sync message
        size_t size = transport_->receive ();
        assert (size == 1);
    }
}
#endif
