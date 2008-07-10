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

#ifndef __PERF_LAT_HPP_INCLUDED__
#define __PERF_LAT_HPP_INCLUDED__

#include <cstdio>
#include "../../transports/i_transport.hpp"
#include "../../../zmq/time.hpp"

namespace perf
{
    void local_lat (i_transport *transport_, size_t msg_size_, 
        int roundtrip_count_)
    {
        // wait for 'remote' side 1B sync message
        size_t size = transport_->receive ();
        assert (size == 1);

        zmq::time_instant_t start_time = zmq::now ();

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++) {

            transport_->send (msg_size_);
            size_t size = transport_->receive ();

            // check incomming message size
            assert (size == msg_size_);
        }

        zmq::time_instant_t stop_time = zmq::now ();
        
        printf ("Your average latency is %.2f us\n", 
            (double)((stop_time - start_time) / 2000) / (double) roundtrip_count_);

        // send sync message
        transport_->send (1);

    }


    void remote_lat (i_transport *transport_, size_t msg_size_, 
        int roundtrip_count_)
    {
//        printf ("remote_lat, msg_size %i, roundtrip_count %i\n", 
//            (int)msg_size_, roundtrip_count_);

        // send sync message to 'local'
        transport_->send (1);

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++)
        {
            size_t size = transport_->receive ();
            assert (size == msg_size_);

            transport_->send (size); 
        }
        
        // wait for sync message
        size_t size = transport_->receive ();
        assert (size == 1);
    }
}
#endif
