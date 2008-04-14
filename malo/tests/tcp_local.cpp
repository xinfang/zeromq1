#include <poll.h>
#include <cstdio>
#include <assert.h>
#include <unistd.h>

#include "../../zmq/tcp_socket.hpp"


using namespace std;

int main (int argc, char *argv[])
{

    zmq::tcp_socket_t s (true, "0.0.0.0", 5555);

    printf ("connect\n");

    pollfd pfd;

    pfd.fd = s.get_fd ();
    pfd.events = POLLOUT;
    
    uint16_t revents;

    unsigned char net_buff [128];


    while (true) {
        printf ("staring poll POLLIN\n");
        int rc = poll (&pfd, 1, -1);
        printf ("polled\n");
        assert (rc != -1);
        assert (!(pfd.revents & (POLLERR | POLLRDHUP | POLLHUP | POLLNVAL)));
        revents = pfd.revents;
        printf ("revents %hx\n", revents);

/*        size_t n_read = s.read (net_buff, sizeof (net_buff));

        printf ("read %zu\n", n_read);

        if (n_read ==0)
            break;
*/
        sleep (1);
    }
    

    return 0;
}
