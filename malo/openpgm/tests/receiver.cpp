#include <assert.h>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>

#include "../../../zmq/pgm_receiver.hpp"

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

    zmq::pgm_receiver_t pgm_receiver (network, port);

//    int fd = pgm_receiver.get_fd ();

    struct iovec* msgv_iov;

    for (int i = 0; i < nloops; i++) {
        
        int rc = pgm_receiver.read_msg (&msgv_iov);
        printf ("read %i B, %s(%i)\n", rc, __FILE__, __LINE__);

        while (rc > 0) {
            for (unsigned int i = 0; i < msgv_iov->iov_len; i++) {
	    	    printf ("[%i]", ((char*)msgv_iov->iov_base) [i]);
                if (((char*)msgv_iov->iov_base) [i] >= '0' && ((char*)msgv_iov->iov_base) [i] <= 'z')
    		        printf ("%c ", ((char*)msgv_iov->iov_base) [i]);    
    	    }

            printf ("\n");
            rc -= msgv_iov->iov_len;
            msgv_iov++;
        }
    }

    return 0;
}
