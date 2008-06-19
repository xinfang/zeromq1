

#include <cstdlib>
#include "../../transports/zmq.hpp"
#include "../scenarios/fo.hpp"

int main (int argc, char *argv [])
{
    if (argc != 6) {
        printf ("Usage: remote_fo <global_locator IP> <global_locator port> "
            "<message size> <message count> <subscriber id>\n");
        return 1;
    }

    const char *g_locator = argv [1];
    unsigned short g_locator_port = atoi (argv [2]);

    size_t msg_size = atoi (argv [3]);
    int roundtrip_count = atoi (argv [4]);
    const char *subs_id = argv [5];


    printf ("subscriber ID: %s\n", subs_id);
    printf ("message size: %i\n", (int)msg_size);
    printf ("roundtrip count: %i\n", roundtrip_count);

    printf ("estimating CPU frequency...\n");
    uint64_t frq = perf::estimate_cpu_frequency ();
    printf ("your CPU frequncy is %.2f GHz\n", ((double) frq) / 1000000000);

    perf::zmq_t transport (true, "QIN", "EOUT", g_locator, g_locator_port, NULL, 0);

    perf::remote_fo (&transport, msg_size, roundtrip_count, subs_id);

}
