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

#ifndef __FI_HPP_INCLUDED__
#define __FI_HPP_INCLUDED__

#include <assert.h>
#include <iostream>
#include <fstream>
#include "../../transports/i_transport.hpp"
#include "../../helpers/time.hpp"

namespace perf
{
    void local_fi (i_transport *transport_, size_t msg_size_, 
          int roundtrip_count_, int pubs_count_)
    {

        std::cout << "waiting for publishers" << std::flush;

        // wait for sync messages from publishers
        for (int pubs_nbr = 0; pubs_nbr < pubs_count_; pubs_nbr++) {
            size_t size = transport_->receive ();
            assert (size == 1);
        }

        std::cout << ".\n";

        // send sync message to publishers (they can start to send messages)
        transport_->send (1);
       
        time_instant_t start_time = 0;

        // receive messages from all publishers
        for (int msg_nbr = 0; msg_nbr < pubs_count_ * roundtrip_count_; msg_nbr++) {
            
            size_t size = transport_->receive ();
            if (msg_nbr == 0)
                start_time = now ();
                
            assert (size == msg_size_);
        }

        time_instant_t stop_time = now (); 
    
        double test_time = (double)(stop_time - start_time) /
            (double) 1000000;

        std::cout.precision (2);

        std::cout << std::fixed << std::noshowpoint <<  "test time: " << test_time << " [ms]\n";

        // throughput [msgs/s]
        unsigned long msg_thput = ((long) 1000000000 *
            (unsigned long) roundtrip_count_) /
            (unsigned long)(stop_time - start_time);
            
        unsigned long tcp_thput = (msg_thput * msg_size_ * 8) /
            (unsigned long) 1000000;
                
        std::cout << std::noshowpoint << "Your average throughput is " << msg_thput << " [msgs/s]\n";
        std::cout << std::noshowpoint << "Your average throughput is " << tcp_thput << " [Mb/s]\n\n";
 
        // save the results
        std::ofstream outf ("tests.dat", std::ios::out | std::ios::app);
        assert (outf.is_open ());
        
        outf.precision (2);

        outf << std::fixed << std::noshowpoint << roundtrip_count_ << "," << msg_size_ << ",";
        outf << std::fixed << std::noshowpoint << test_time << "," << msg_thput << "," << tcp_thput << std::endl;
        
        outf.close ();

        // send sysnc message to publishers
        transport_->send (1);
    }

    void remote_fi (i_transport *transport_, size_t msg_size_, 
          int roundtrip_count_)
    {
        // send sync message to subscriber
        transport_->send (1); 

        // wait for sync message
        size_t size = transport_->receive ();
        assert (size == 1);

        // send bunch of messages
        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++) {
            transport_->send (msg_size_);
        }

        // wait for sync message
        size = transport_->receive ();
        assert (size == 1);
    }
}
#endif
