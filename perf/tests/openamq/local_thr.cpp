
#include <cstdlib>
#include <cstdio>

#include "../../transports/openamq.hpp"
#include "../../workers/raw_receiver.hpp"
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"

int main (int argc, char *argv [])
{
    assert (argc == 2);
    perf::openamq_t transport (argv [1]);
    perf::raw_receiver_t worker (100000);
    worker.run (transport);
    perf::time_instant_t start_time, stop_time;
    perf::read_times_1f (&start_time, &stop_time, "in.dat");
    int throughput =
        (int) (100000 / (((double) (stop_time - start_time)) /  1000000));
    printf ("Throughput: %d msgs/sec\n", throughput);
    return 0;
}

