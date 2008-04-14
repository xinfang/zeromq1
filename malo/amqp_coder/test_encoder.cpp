//#include "../../zmq/amqp09_encoder.hpp"
#include "../../zmq/amqp09_server_socket.hpp"
#include "../../zmq/dispatcher.hpp"
#include "../../zmq/io_thread.hpp"
#include "../../zmq/dispatcher_proxy.hpp"
#include "../../zmq/api_thread.hpp"
#include "../../zmq/tcp_socket.hpp"

using namespace zmq;

int main ()
{
    dispatcher_t dispatcher (2);
    amqp09_server_socket_t socket ("0.0.0.0", 5672);
//    tcp_socket_t socket ( true, "0.0.0.0", 5672);
    io_thread_t io ( &dispatcher, 0, 1, 1, &socket, 8192, 8192);
    api_thread_t api_thread (&dispatcher, 1);

    cmsg_t msg;// = {(void*)"123", 3, NULL};

    // 10kB
    msg.data = malloc (10240);
    msg.size = 10240;
    msg.ffn = free;
    
    assert (msg.data);

    memset (msg.data, '-', 5120);
    memset ((unsigned char*)msg.data + 5120 , '|', 5120);

    api_thread.send (0, msg);

    sleep (1);

    return 0;
}

