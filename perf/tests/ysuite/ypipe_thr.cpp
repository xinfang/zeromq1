#include <unistd.h>

#include "../../transports/ysuite.hpp"
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"
#include "../../workers/raw_receiver.hpp"
#include "../../workers/raw_sender.hpp"

#include "test.hpp"

void *worker_function (void*);

struct worker_args_t
{
    int id;
    int msg_size;
    int msg_count;
    perf::i_transport *transport;
};

int main (void) {

    perf::ysuite_t transport (perf::active_sync_semaphore);
    perf::raw_receiver_t receiver (TEST_MSG_COUNT_THRPUT);

    worker_args_t w_args;
    w_args.id = 0;
    w_args.msg_size = 0;
    w_args.msg_count = TEST_MSG_COUNT_THRPUT;
    w_args.transport = &transport;

    pthread_t worker;

    int rc = pthread_create (&worker, NULL, worker_function, 
            (void*)&w_args);
        assert (rc == 0);


    receiver.run (transport, "");

    rc = pthread_join (worker, NULL);
    assert (rc == 0);

    perf::time_instant_t start_time;
    perf::time_instant_t stop_time;

    perf::read_times_1f (&start_time, &stop_time, "in.dat");

    printf ("Test time: %llu [ms]\n", (stop_time - start_time) / 
            (long long)1000);

    // throughput [msgs/s]
    unsigned long long msg_thput = ((long long) 1000000 * (long long) TEST_MSG_COUNT_THRPUT ) / 
            (stop_time - start_time);

    printf ("Your average throughput is %llu msgs/s\n", msg_thput);

    return 0;
}


void *worker_function (void *args_)
{
    // args struct
    worker_args_t *w_args = (worker_args_t*)args_;
    
    perf::raw_sender_t sender (w_args->msg_count, w_args->msg_size);

    sender.run (*w_args->transport, "");
}

