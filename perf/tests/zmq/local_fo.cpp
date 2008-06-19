
#include <cstdlib>
#include "../../transports/zmq.hpp"
#include "../scenarios/fo.hpp"

int main (int argc, char *argv [])
{
    
    if (argc != 8) {
        printf ("Usage: local_fo <global_locator IP> <global_locator port> "
            "<listen IP> <listen port> <message size> <message count> "
            "<number of subscribers>\n");
        return 1;
    }

    size_t msg_size = atoi (argv [5]);
    int roundtrip_count = atoi (argv [6]);
    int subs_count = atoi (argv [7]);

    printf ("subcribers: %i\n", subs_count);
    printf ("message size: %i\n", (int)msg_size);
    printf ("roundtrip count: %i\n", roundtrip_count);

    perf::zmq_t transport (false, "QIN", "EOUT", argv [1], atoi (argv [2]), 
        argv [3], atoi (argv [4]));
   
    perf::local_fo (&transport, msg_size, roundtrip_count, subs_count);

    return 0;
}
