#include <iostream>
#include <zmq.hpp>

int main (int argc, char *argv [])
{
 
    if (argc != 3) {
        std::cerr << "Usage: router_test_client <hostname> <identity>" << std::endl;
        return 1;
    }

    const char *host = argv [1];
    const char *identity = argv [2];

    zmq::dispatcher_t dispatcher (2);
    zmq::locator_t locator (host);
 
    zmq::api_thread_t *api = zmq::api_thread_t::create (&dispatcher, &locator);
    zmq::i_thread *worker = zmq::io_thread_t::create (&dispatcher);

    api->create_queue (identity);
    api->bind ("E", identity, worker, worker);

    while (true) {
        zmq::message_t msg;
        api->receive (&msg);
        std::cout << "." << std::flush;
    }

    return 0;
}
