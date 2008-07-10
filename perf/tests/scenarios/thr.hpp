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

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <pthread.h>

#include "../../transports/i_transport.hpp"
#include "../../../zmq/time.hpp"

namespace perf
{
    struct thr_worker_args_t
    {
        i_transport *transport;
        size_t msg_size;
        int roundtrip_count;

        zmq::time_instant_t start_time;
        zmq::time_instant_t stop_time;
    };

    template <typename T> inline std::string to_string (const T &input_)
    {
        std::stringstream string_stream;
        string_stream << input_;
        return string_stream.str ();
    }

    void *local_worker_function (void *worker_args_)
    {
        thr_worker_args_t *args = (thr_worker_args_t*)worker_args_;

        for (int msg_nbr = 0; msg_nbr < args->roundtrip_count; msg_nbr++)
        {
            
            size_t size = args->transport->receive ();
            if (msg_nbr == 0)
                args->start_time  = zmq::now ();
            
            //  Check incomming message size
            assert (size == args->msg_size);
        }

        args->stop_time = zmq::now();

        //  Send sync message
        args->transport->send (1);

        return NULL;
    }

    void *remote_worker_function (void *worker_args_)
    {

        perf::thr_worker_args_t *args = (thr_worker_args_t*)worker_args_;

        for (int msg_nbr = 0; msg_nbr < args->roundtrip_count; msg_nbr++)
        {
            args->transport->send (args->msg_size);
        }

        //  Wait for sync message
        size_t size = args->transport->receive ();
        assert (size == 1);

        return NULL;
    }

    void local_thr (i_transport **transports_, size_t msg_size_, 
        int roundtrip_count_, int thread_count_)
    {

        pthread_t *workers = new pthread_t [thread_count_];
        perf::thr_worker_args_t *workers_args = 
            new perf::thr_worker_args_t [thread_count_];

        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {

            workers_args [thread_nbr].transport = transports_ [thread_nbr];
            workers_args [thread_nbr].msg_size = msg_size_;
            workers_args [thread_nbr].roundtrip_count = roundtrip_count_;
            workers_args [thread_nbr].start_time = 0;
            workers_args [thread_nbr].stop_time = 0;
            
            int rc = pthread_create (&workers [thread_nbr], NULL, 
                local_worker_function, (void *)&workers_args [thread_nbr]);
            assert (rc == 0);
        }

        //  Gather results from thr_worker_args_t structures
        zmq::time_instant_t min_start_time  = 
            std::numeric_limits<uint64_t>::max ();
        zmq::time_instant_t max_stop_time = 0;

        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {
            int rc = pthread_join (workers [thread_nbr], NULL);
            assert (rc == 0);

            if (workers_args [thread_nbr].start_time < min_start_time)
                min_start_time = workers_args [thread_nbr].start_time;

            if (workers_args [thread_nbr].stop_time > max_stop_time)
                max_stop_time = workers_args [thread_nbr].stop_time;

        }

        delete [] workers_args;
        delete [] workers;

        double test_time = (double)(max_stop_time - min_start_time) /
            (double) 1000000;

        std::cout.precision (2);

        std::cout << std::fixed << std::noshowpoint <<  "test time: " 
            << test_time << " [ms]\n";

        //  Throughput [msgs/s]
        unsigned long msg_thput = ((long) 1000000000 *
            (unsigned long) roundtrip_count_ * (unsigned long) thread_count_)/
            (unsigned long)(max_stop_time - min_start_time);
            
        unsigned long tcp_thput = (msg_thput * msg_size_ * 8) /
            (unsigned long) 1000000;
                
        std::cout << std::noshowpoint << "Your average throughput is " 
            << msg_thput << " [msg/s]\n";
        std::cout << std::noshowpoint << "Your average throughput is " 
            << tcp_thput << " [Mb/s]\n\n";
 
        //  Save the results
        std::ofstream outf ("tests.dat", std::ios::out | std::ios::app);
        assert (outf.is_open ());
        
        outf.precision (2);

        outf << std::fixed << std::noshowpoint << thread_count_ << "," << roundtrip_count_ 
            << "," << msg_size_ << "," << test_time << "," << msg_thput << "," 
            << tcp_thput << std::endl;
        
        outf.close ();

    }

    void remote_thr (i_transport **transports_, size_t msg_size_, 
        int roundtrip_count_, int thread_count_)
    {
        pthread_t *workers = new pthread_t [thread_count_];
        perf::thr_worker_args_t *workers_args = 
            new perf::thr_worker_args_t [thread_count_];

        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {

            workers_args [thread_nbr].transport = transports_ [thread_nbr];
            workers_args [thread_nbr].msg_size = msg_size_;
            workers_args [thread_nbr].roundtrip_count = roundtrip_count_;
            workers_args [thread_nbr].start_time = 0;
            workers_args [thread_nbr].stop_time = 0;
            
            int rc = pthread_create (&workers [thread_nbr], NULL, 
                remote_worker_function, (void *)&workers_args [thread_nbr]);
            assert (rc == 0);
        }

        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {
            int rc = pthread_join (workers [thread_nbr], NULL);
            assert (rc == 0);
        }
    }

}
#endif
