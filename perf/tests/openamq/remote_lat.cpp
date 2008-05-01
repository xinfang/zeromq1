
#include "../../transports/openamq.hpp"
#include "../../workers/echo.hpp"

int main (int argc, char *argv [])
{
    assert (argc == 2);
    perf::openamq_t transport (argv [1]);
    perf::echo_t worker (100000);
    worker.run (transport);
    return 0;
}
