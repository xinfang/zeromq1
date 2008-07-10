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

#ifndef __FO_HPP_INCLUDED__
#define __FO_HPP_INCLUDED__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "../../transports/i_transport.hpp"
#include "../../../zmq/time.hpp"

namespace perf
{
    void local_fo (i_transport *transport_, size_t msg_size_, 
        int roundtrip_count_, int subs_count)
    {

        for (int subs_nbr = 0; subs_nbr < subs_count; subs_nbr++) {
            size_t size = transport_->receive ();
            assert (size == 1);
        }

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++) {
            transport_->send (msg_size_);
        }
        
        for (int subs_nbr = 0; subs_nbr < subs_count; subs_nbr++) {
            size_t size = transport_->receive ();
            assert (size == 1);
        }
        
        transport_->send (1);

    }

    void remote_fo (i_transport *transport_, size_t msg_size_,
        int roundtrip_count_, const char *subs_id_)
    {
        transport_->send (1);

        zmq::time_instant_t start_time = 0;

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++) {

            size_t size = transport_->receive ();
            if (msg_nbr == 0)
                start_time = zmq::now();

            //  Check incomming message size
            assert (size == msg_size_);
        }

        zmq::time_instant_t stop_time = zmq::now();

        double test_time = (double)(stop_time - start_time) /
            (double) 1000000;

        std::cout.precision (2);

        std::cout << std::fixed << std::noshowpoint <<  "test time: "
            << test_time << " [ms]\n";

        //  Throughput [msgs/s]
        unsigned long msg_thput = ((long) 1000000000 *
            (unsigned long) roundtrip_count_) /
            (unsigned long)(stop_time - start_time);
            
        unsigned long tcp_thput = (msg_thput * msg_size_ * 8) /
            (unsigned long) 1000000;
                
        std::cout << std::noshowpoint << "Your average throughput is "
            << msg_thput << " [msg/s]\n";
        std::cout << std::noshowpoint << "Your average throughput is "
            << tcp_thput << " [Mb/s]\n\n";
           
        //  Save the results
        std::string _filename (subs_id_);
        _filename += "_tests.dat";

        std::ofstream outf (_filename.c_str (), std::ios::out | std::ios::app);
        assert (outf.is_open ());
        
        outf.precision (2);

        outf << std::fixed << std::noshowpoint << "1," << roundtrip_count_ 
            << "," << msg_size_ << "," << test_time << "," 
            << msg_thput << "," << tcp_thput << std::endl;
        
        outf.close ();

        transport_->send (1);    

        //  All consumers finished, now we can shutdown
        size_t size = transport_->receive ();
        assert (size == 1);
    }
}
#endif
