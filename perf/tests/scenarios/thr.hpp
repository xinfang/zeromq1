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

/*

    Message flow diagram

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
#include <pthread.h>

#include "../../transports/i_transport.hpp"
#include "../../../zmq/time.hpp"

namespace perf
{
    // Worker thread arguments structure
    struct thr_worker_args_t
    {
        // Transport beeing used by the worker, it has to be created in advance
        i_transport *transport;
        // Size of the message being transported in the test
        size_t msg_size;
        // Number of the messages in the test
        int roundtrip_count;

        // Timestamps captured by the worker thread at the beggining & end 
        // of the test
        zmq::time_instant_t start_time;
        zmq::time_instant_t stop_time;
    };

    // 'local' worker thread function
    void *local_worker_function (void *worker_args_)
    {
        thr_worker_args_t *args = (thr_worker_args_t*)worker_args_;

        // Receive msg_nbr messages of msg_size
        for (int msg_nbr = 0; msg_nbr < args->roundtrip_count; msg_nbr++)
        {
            size_t size = args->transport->receive ();

            // Capture arrival timestamp of the first message (test start)
            if (msg_nbr == 0)
                args->start_time  = zmq::now ();
            
            //  Check incomming message size
            assert (size == args->msg_size);
        }

        // Capture test stop timestamp
        args->stop_time = zmq::now();

        //  Send sync message to the peer
        args->transport->send (1);

        return NULL;
    }

    // 'remote' worker thread function
    void *remote_worker_function (void *worker_args_)
    {
        perf::thr_worker_args_t *args = (thr_worker_args_t*)worker_args_;

        // Send msg_nbr messages of msg_size
        for (int msg_nbr = 0; msg_nbr < args->roundtrip_count; msg_nbr++)
        {
            args->transport->send (args->msg_size);
        }

        //  Wait for sync message
        size_t size = args->transport->receive ();
        assert (size == 1);

        return NULL;
    }

    // Function initializes parameter structure for each thread and starts
    // local_worker_function(s) in separate thread(s).
    void local_thr (i_transport **transports_, size_t msg_size_, 
        int roundtrip_count_, int thread_count_)
    {
        pthread_t *workers = new pthread_t [thread_count_];

        // Array of thr_worker_args_t structures for worker threads
        perf::thr_worker_args_t *workers_args = 
            new perf::thr_worker_args_t [thread_count_];

        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {
            // Fill structure, note that start_time & stop_time is filled 
            // by worker thread at the begining & end of the test
            workers_args [thread_nbr].transport = transports_ [thread_nbr];
            workers_args [thread_nbr].msg_size = msg_size_;
            workers_args [thread_nbr].roundtrip_count = roundtrip_count_;
            workers_args [thread_nbr].start_time = 0;
            workers_args [thread_nbr].stop_time = 0;
            
            // Create worker thread
            int rc = pthread_create (&workers [thread_nbr], NULL, 
                local_worker_function, (void *)&workers_args [thread_nbr]);
            assert (rc == 0);
        }

        //  Gather results from thr_worker_args_t structures
        zmq::time_instant_t min_start_time  = 
            std::numeric_limits<uint64_t>::max ();
        zmq::time_instant_t max_stop_time = 0;

        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {
            // Wait for worker threads to finish
            int rc = pthread_join (workers [thread_nbr], NULL);
            assert (rc == 0);

            // Find max stop & min start time
            if (workers_args [thread_nbr].start_time < min_start_time)
                min_start_time = workers_args [thread_nbr].start_time;

            if (workers_args [thread_nbr].stop_time > max_stop_time)
                max_stop_time = workers_args [thread_nbr].stop_time;

        }

        delete [] workers_args;
        delete [] workers;

        // Calculate results
        double test_time = (double)(max_stop_time - min_start_time) /
            (double) 1000000;

        std::cout.precision (2);

        std::cout << std::fixed << std::noshowpoint <<  "test time: " 
            << test_time << " [ms]\n";

        // Throughput [msgs/s]
        unsigned long msg_thput = ((long) 1000000000 *
            (unsigned long) roundtrip_count_ * (unsigned long) thread_count_)/
            (unsigned long)(max_stop_time - min_start_time);

        // Throughput [B/s]
        unsigned long tcp_thput = (msg_thput * msg_size_ * 8) /
            (unsigned long) 1000000;
                
        std::cout << std::noshowpoint << "Your average throughput is " 
            << msg_thput << " [msg/s]\n";
        std::cout << std::noshowpoint << "Your average throughput is " 
            << tcp_thput << " [Mb/s]\n\n";
 
        //  Save the results into tests.dat file
        std::ofstream outf ("tests.dat", std::ios::out | std::ios::app);
        assert (outf.is_open ());
        
        outf.precision (2);

        // Output file format, separate line for each run is appended 
        // to the tests.dat file
        //
        // thread count, roundtrip count, msg size, test time, 
        //   throughput [msg/s],throughput [B/s]
        //
        outf << std::fixed << std::noshowpoint << thread_count_ << "," 
            << roundtrip_count_ << "," << msg_size_ << "," << test_time << "," 
            << msg_thput << "," << tcp_thput << std::endl;
        
        outf.close ();

    }

    // Function initializes parameter structure for each thread and starts
    // remote_worker_function(s) in separate thread(s).
    void remote_thr (i_transport **transports_, size_t msg_size_, 
        int roundtrip_count_, int thread_count_)
    {
        pthread_t *workers = new pthread_t [thread_count_];

        // Array of thr_worker_args_t structures for worker threads
        perf::thr_worker_args_t *workers_args = 
            new perf::thr_worker_args_t [thread_count_];


        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {
            // Fill structures
            workers_args [thread_nbr].transport = transports_ [thread_nbr];
            workers_args [thread_nbr].msg_size = msg_size_;
            workers_args [thread_nbr].roundtrip_count = roundtrip_count_;
            workers_args [thread_nbr].start_time = 0;
            workers_args [thread_nbr].stop_time = 0;
         
            // Create worker thread
            int rc = pthread_create (&workers [thread_nbr], NULL, 
                remote_worker_function, (void *)&workers_args [thread_nbr]);
            assert (rc == 0);
        }

        // Wait for worker threads to finish
        for (int thread_nbr = 0; thread_nbr < thread_count_; thread_nbr++) {
            int rc = pthread_join (workers [thread_nbr], NULL);
            assert (rc == 0);
        }
    }

}
#endif
