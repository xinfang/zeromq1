#include <assert.h>
#include <cstdio>
#include <unistd.h>

#include "../../zmq/epgm_sender.hpp"

int main (int argc, char *argv [])
{

    zmq::epgm_sender_t pgm_sender ("eth3;226.0.0.1", 7500);


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

    for (int i = 0; i < 10; i++) {
        int rc = pgm_sender.write_pkt ((unsigned char*)b_buff, strlen (b_buff), i - 1);
//       sleep (1);
    }

    return 0;
}
