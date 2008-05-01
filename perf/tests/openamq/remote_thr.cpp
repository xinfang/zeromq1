
#include <cstdlib>
#include <cstdio>

#include "../../transports/openamq.hpp"
#include "../../workers/raw_sender.hpp"

int main (int argc, char *argv [])
{
    assert (argc == 2);
    perf::openamq_t transport (argv [1]);
    perf::raw_sender_t worker (100000, 6);
    worker.run (transport);

    return 0;
}

