#include <assert.h>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>

#include "../../../zmq/pgm_socket.hpp"

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

    size_t readbuf_size = 8192;

    zmq::pgm_socket_t pgm_socket (true, false, network, port, readbuf_size);

    size_t iov_len = pgm_socket.get_max_apdu_at_once (readbuf_size);

    iovec *iov = new iovec [iov_len];


    while (nloops) {
        
        int nbytes = pgm_socket.read_pkt (iov, iov_len);
        printf ("read %i B, %s(%i)\n", nbytes, __FILE__, __LINE__);

/*        if (nbytes > 0) {
            for (unsigned int i = 0; i < iov.iov_len; i++) {
	    	    printf ("[%i]", ((char*)iov.iov_base) [i]);
                if (((char*)iov.iov_base) [i] >= '0' && ((char*)iov.iov_base) [i] <= 'z')
    		        printf ("%c ", ((char*)iov.iov_base) [i]);    
    	    }
            printf ("\n");

            nloops--;
        }
*/
        sleep (1);

    }

    delete [] iov;

    return 0;
}
