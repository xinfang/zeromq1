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

#ifndef __PERF_THR_LAT_HPP_INCLUDED__
#define __PERF_THR_LAT_HPP_INCLUDED__

#include <assert.h>
#include <fstream>

#include "../../transports/i_transport.hpp"
#include "../../helpers/ticker.hpp"

namespace perf
{

    void local_thr_lat (i_transport **transports_, size_t msg_size_, 
        int roundtrip_count_, int thread_count_)
    {
        
        zmq::estimate_cpu_frequency ();
        std::cout << "ready" << std::endl;

        zmq::time_instant_t *stop_times = new zmq::time_instant_t [roundtrip_count_];

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++)
        {
            
            size_t size = transports_[0]->receive ();
            stop_times [msg_nbr] = zmq::now ();

            // check incomming message size
            assert (size == msg_size_);
        }

        std::cout << "writting results..." << std::flush;

        // write stop_times into the file
        std::ofstream outf ("stop_times.dat", std::ios::out | std::ios::app);
        assert (outf.is_open ());

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++)
        {
            outf << stop_times [msg_nbr] << std::endl;
        }

        outf.close ();
        std::cout << "done" << std::endl;

        // calculate and display throughput
        unsigned long msg_thput = ((long) 1000000000 *
            (unsigned long) roundtrip_count_ * (unsigned long) thread_count_)/
            (unsigned long)(stop_times [roundtrip_count_ - 1] - stop_times [0]);
            
        unsigned long tcp_thput = (msg_thput * msg_size_ * 8) /
            (unsigned long) 1000000;
                
        std::cout << std::noshowpoint << "Your average throughput is " 
            << msg_thput << " [msg/s]\n";
        std::cout << std::noshowpoint << "Your average throughput is " 
            << tcp_thput << " [Mb/s]\n\n";

        // send sync message
        transports_[0]->send (1);

        // wait for peer to write start_times
        size_t size = transports_[0]->receive ();
        assert (size == 1);

        delete [] stop_times;
    }

    void remote_thr_lat (i_transport **transports_, size_t msg_size_, 
        int roundtrip_count_, int thread_count_, int msgs_per_second_)
    {

        ticker_t ticker (msgs_per_second_); 

        zmq::time_instant_t *start_times = new zmq::time_instant_t [roundtrip_count_];

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++)
        { 
            ticker.wait_for_tick ();
            start_times [msg_nbr] = zmq::now ();
            transports_[0]->send (msg_size_);
        }

        // wait for sync message
        size_t size = transports_[0]->receive ();
        assert (size == 1);


        // write start_times into the file
        std::cout << "writting results..." << std::flush;
        std::ofstream outf ("start_times.dat", std::ios::out | std::ios::app);
        assert (outf.is_open ());

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++)
        {
            outf << start_times [msg_nbr] << std::endl;
        }

        outf.close ();

        std::cout << "done" << std::endl;

        // send sync message
        transports_[0]->send (1);

        delete [] start_times;
    }
}
#endif
