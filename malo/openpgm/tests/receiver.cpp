#include <assert.h>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>

#include "../../../zmq/epgm_socket.hpp"

int main (int argc, char *argv [])
{
    if (argc < 4) {
        printf ("usage: receiver network port nloops\n");
        return 1;
    }

    const char *network = argv [1];
    int port = atoi (argv [2]);

    int nloops = atoi (argv [3]);

    printf ("network \"%s\", port %i, nloops %i\n", 
        network, port, nloops);

    zmq::epgm_socket_t pgm_socket (true, false, network, port);

//    int fd = pgm_receiver.get_fd ();

    iovec iov;

    while (nloops) {
        
        int nbytes = pgm_socket.read_one_pkt_with_offset (&iov);
        printf ("read %i B, %s(%i)\n", nbytes, __FILE__, __LINE__);

        if (nbytes > 0) {
            for (unsigned int i = 0; i < iov.iov_len; i++) {
	    	    printf ("[%i]", ((char*)iov.iov_base) [i]);
                if (((char*)iov.iov_base) [i] >= '0' && ((char*)iov.iov_base) [i] <= 'z')
    		        printf ("%c ", ((char*)iov.iov_base) [i]);    
    	    }
            printf ("\n");

            nloops--;
        }
        sleep (1);
    }

    return 0;
}
