





#ifndef __FO_HPP_INCLUDED__
#define __FO_HPP_INCLUDED__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "../../transports/i_transport.hpp"
#include "../../helpers/time.hpp"

namespace perf
{
    
    void local_fo (i_transport *transport, size_t msg_size_, 
        int roundtrip_count_, int subs_count)
    {

        printf ("waiting for subsribers");
        fflush (stdout);
        for (int subs_nbr = 0; subs_nbr < subs_count; subs_nbr++) {
            size_t size = transport->receive ();
            assert (size == 1);
//            printf (".");
        }

//        printf ("\n");

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++) {
            transport->send (msg_size_);
        }
        
        printf ("waititng for subscribers to finish");
        fflush (stdout);
        for (int subs_nbr = 0; subs_nbr < subs_count; subs_nbr++) {
            size_t size = transport->receive ();
            assert (size == 1);
//            printf ("-");
        }
//        printf ("\n");
    }

    void remote_fo (i_transport *transport, size_t msg_size_,
        int roundtrip_count_, const char *subs_id_)
    {
        printf ("sending sync message\n");
        transport->send (1);

        time_instant_t start_time = 0;

        for (int msg_nbr = 0; msg_nbr < roundtrip_count_; msg_nbr++) {
            size_t size = transport->receive ();
            if (msg_nbr == 0)
                start_time = now();

            // check incomming message size
            assert (size == msg_size_);
        }

        time_instant_t stop_time = now();

//        std::cout << start_time << " " << stop_time << " " << stop_time - start_time << std::endl;

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
                
        std::cout << std::noshowpoint <<  "Your average throughput is " << msg_thput << " [msgs/s]\n";
        std::cout << std::noshowpoint << "Your average throughput is " << tcp_thput << " [Mb/s]\n\n";
           
        // save the results
        std::string _filename (subs_id_);
        _filename += "_tests.dat";

        std::ofstream outf (_filename.c_str (), std::ios::out | std::ios::app);
        assert (outf.is_open ());
        
        outf.precision (2);

        outf << std::fixed << std::noshowpoint << subs_id_ << "," << roundtrip_count_ << "," << msg_size_ << ",";
        outf << std::fixed << std::noshowpoint << test_time << "," << msg_thput << "," << tcp_thput << std::endl;
        
        outf.close ();

        printf ("sending sync message\n");
        transport->send (1);    

        // all consumers finished, now we can shutdown
        size_t size = transport->receive ();
        assert (size == 1);
    }
}
#endif
