
#include "tcp_socket.h"


int main (void) {
    zmq::tcp_socket_t socket (false, "127.0.0.1", 555);

    return 0;
}
