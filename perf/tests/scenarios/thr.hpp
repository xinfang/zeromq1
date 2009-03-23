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

/*

    Message flow diagram for throughput scenario

          'local'                   'remote'
      (started first)          (started second)
             |
             |                        
             |                         |
             |  messages (size,count)  | 
             |<========================|
             |                         |
             | sync message (size 1B)  |
             |------------------------>|
             |                         |
             |                         v
      resuls gathering 
        computations
             |
             v

*/

#ifndef __PERF_THR_HPP_INCLUDED__
#define __PERF_THR_HPP_INCLUDED__

#include <iostream>
#include <fstream>
#include <limits>

#include "../../transports/i_transport.hpp"
#include "../../helpers/time.hpp"

namespace perf
{

    //  Function initializes parameter structure for each thread and starts
    //  local_worker_function(s) in separate thread(s).
    void local_thr (i_transport *transport_, size_t msg_size_, 
        int msg_count_)
    {
        
        //  Timestamp captured after receiving first message.
        time_instant_t start_time = 0;

        //  Receive msg_nbr messages of msg_size.
        for (int msg_nbr = 0; msg_nbr < msg_count_; msg_nbr++)
        {
            size_t size = transport_->receive ();

            //  Capture arrival timestamp of the first message (test start).
            if (msg_nbr == 0)
                start_time  = now ();
            
            //  Check incomming message size.
            assert (size == msg_size_);
        }

        //  Capture test stop timestamp.
        time_instant_t stop_time = now();
        
        //  Send sync message to the peer.
        transport_->send (1);

        //  Calculate results.

        //  Test time in [ms] with [ms] resolution, do not use for math!!!
        uint64_t test_time = uint64_t (stop_time - start_time) /
            (uint64_t) 1000000;
                
        //  Throughput [msgs/s].
        uint64_t msg_thput = ((uint64_t) 1000000000 *
            (uint64_t) msg_count_) / (uint64_t) (stop_time - start_time);

        //  Throughput [Mb/s].
        uint64_t tcp_thput = (msg_thput * msg_size_ * 8) /
            (uint64_t) 1000000;
                
        std::cout << "Your average throughput is " << msg_thput 
            << " [msg/s]" << std::endl;
        std::cout << "Your average throughput is " << tcp_thput 
            << " [Mb/s]" << std::endl << std::endl;
 
        //  Save the results into tests.dat file.
        std::ofstream outf ("tests.dat", std::ios::out | std::ios::app);
        assert (outf.is_open ());
        
        //  Output file format, separate line for each run is appended 
        //  to the tests.dat file.
        //
        //  thread count, message count, msg size [B], test time [ms],
        //  throughput [msg/s],throughput [Mb/s]
        //
        outf << "1" << "," << msg_count_ << "," << msg_size_ << "," 
            << test_time << "," << msg_thput << "," << tcp_thput << std::endl;
        
        outf.close (); 
    }

    //  Function initializes parameter structure for each thread and starts
    //  remote_worker_function(s) in separate thread(s).
    void remote_thr (i_transport *transport_, size_t msg_size_, 
        int msg_count_)
    {

        //  Send msg_nbr messages of msg_size.
        for (int msg_nbr = 0; msg_nbr < msg_count_; msg_nbr++)
        {
            transport_->send (msg_size_);
        }

        //  Wait for sync message.
        size_t size = transport_->receive ();
        assert (size == 1);
    }
}
#endif
