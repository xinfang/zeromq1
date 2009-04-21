#include <iostream>
#include <zmq.hpp>

struct hint_t {
    char hint;
    const char *dst;
};

bool router_fcn (zmq::message_t *msg_, const char *remote_object_)
{ 
    hint_t *hints = (hint_t*)msg_->get_hint ();
    assert (hints); 

    while (hints->hint) {
        const char *msg_data = (const char*)msg_->data ();
        
        if (msg_data [0] == hints->hint && strcmp (remote_object_, hints->dst) == 0) {
            return true;
        }
 
        hints++;
    }

    return false;
}

int main (int argc, char *argv [])
{

    if (argc != 3) {
        std::cerr << "Usage: router_test <hostname> <exchange interface>" << std::endl;
        return 1;
    }

    hint_t hints [] = {
        {'A', "Q1"},
        {'B', "Q2"},
        {'\0', NULL}
    };

    const char *host = argv [1];
    const char *exchange_interface = argv [2];

    zmq::dispatcher_t dispatcher (2);
    zmq::locator_t locator (host);
 
    zmq::api_thread_t *api = zmq::api_thread_t::create (&dispatcher, &locator);
    zmq::i_thread *worker = zmq::io_thread_t::create (&dispatcher);

    int exchange_id = api->create_exchange ("E", zmq::scope_global, 
        exchange_interface, worker, 1, &worker);

    char payload_1 [] = "A message 1";
    char payload_2 [] = "B message 2";

    std::cout << "Press enter to send messages." << std::endl;
    getchar (); 

    zmq::set_routing_handler (router_fcn);

    zmq::message_t msg_1 (payload_1, sizeof (payload_1), NULL);
    msg_1.set_hint (hints);
    api->send (exchange_id, msg_1, false);

    zmq::message_t msg_2 (payload_2, sizeof (payload_2), NULL);
    msg_2.set_hint (hints);
    api->send (exchange_id, msg_2, false);

    std::cout << "Press enter to exit." << std::endl;
    getchar (); 

    return 0;
}
