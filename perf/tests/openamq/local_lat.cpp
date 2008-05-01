
#include <stdio.h>

#include "../../transports/openamq.hpp"
#include "../../workers/raw_ping_pong.hpp"
#include "../../helpers/time.hpp"
#include "../../helpers/files.hpp"

int main (int argc, char *argv [])
{
    assert (argc == 2);

    perf::openamq_t transport ("127.0.0.1");
    perf::raw_ping_pong_t worker (100000, 6);
    worker.run (transport);
    perf::time_instant_t start_time, stop_time;
    perf::read_times_2f (&start_time, &stop_time, "");
    double latency = ((double) (stop_time - start_time)) / 200000;
    printf ("Latency: %f us\n", latency);
    return 0;
}
