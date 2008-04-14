#include <poll.h>
#include <cstdio>

#include "../../zmq/tcp_socket.hpp"


using namespace std;

int main (int argc, char *argv[])
{

    if (argc != 2) {
        printf ("IP address is missing\n");
        return 1;
    }


    zmq::tcp_socket_t s (false, argv [1], 5555);

    printf ("connect\n");


    getchar ();

    return 0;
}
