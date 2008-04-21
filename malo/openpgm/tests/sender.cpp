#include <assert.h>
#include <cstdio>
#include <unistd.h>

#include "../../../zmq/epgm_socket.hpp"

int main (int argc, char *argv [])
{

    zmq::epgm_socket_t pgm_socket (false, false, "eth3;226.0.0.1", 7500);


//    char a_buff[] = "AB";
    char b_buff[] = "abcdefghijk";


/*    struct iovec iov [2];
    iov [0].iov_base = a_buff;
    iov [0].iov_len = strlen (a_buff);
    iov [1].iov_base = b_buff;
    iov [1].iov_len = strlen (b_buff);
*/
//    memset (buff, '-', sizeof (buff));
//    snprintf (buff, sizeof (buff), "abcdefghijk"); 

    size_t tsdu = 0;

    for (int i = 0; i < 10; i++) {
        unsigned char *buff = pgm_socket.alloc_one (&tsdu);

        printf ("received %i in from tx window\n", tsdu);
        strcpy ((char*)buff, "__abcdefg");
        int rc = pgm_socket.write_pkt (buff, strlen ((char*)buff), i - 1);
       sleep (2);
    }

    return 0;
}
