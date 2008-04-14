#include "../../zmq/amqp09_decoder.hpp"
#include "../../zmq/dispatcher.hpp"
#include "../../zmq/dispatcher_proxy.hpp"

using namespace zmq;

int main ()
{
    dispatcher_t dispatcher (1);
    dispatcher_proxy_t proxy (&dispatcher, 0, NULL);
    amqp09_decoder_t decoder (63536, &proxy, 0);

/*    {
        unsigned char header [] = {1, 0, 1, 0, 0, 0, 24};
        decoder.write (header, 7, NULL);

        unsigned char payload [] = {0, 60, 0, 40, 0, 0, 8, 115, 101, 114, 
              118, 105, 99, 101, 115, 7, 109, 101, 115, 115, 97, 103, 101, 3, 206};
        decoder.write (payload, 25, NULL);
    }

    {
        unsigned char header [] = {2, 0, 1, 0, 0, 0, 34};
        decoder.write (header, 7, NULL);

        unsigned char payload [] = {0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 2, 128, 
              13, 114, 101, 112, 108, 121, 95, 116, 111, 95, 112, 114, 111, 112, 
              5, 73, 68, 48, 0, 48, 0, 49, 1, 206};
        decoder.write (payload, 35, NULL);

    }

    {
        unsigned char header [] = {3, 0, 1, 0, 0, 0, 10};
        decoder.write (header, 7, NULL);

        unsigned char payload [] = {71, 69, 84, 58, 32, 73, 116, 32, 105, 115, 206};
        decoder.write (payload, 11, NULL);
    }

    {
        unsigned char header [] = {3, 0, 1, 0, 0, 0, 7};
        decoder.write (header, 7, NULL);

        unsigned char payload [] = {32, 109, 101, 32, 97, 97, 46, 206};
        decoder.write (payload, 8, NULL);
    }
*/   

/*
    {
        unsigned char header [] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x09};
        decoder.write (header, 7);

        unsigned char payload [] = {0x00, 0x3c, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0xce};
        decoder.write (payload, 10);
    }

    {
        unsigned char header [] = {0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x4c};
        decoder.write (header, 7);

        unsigned char payload [] = {0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x06, 0x20, 0x00, 0x00, 0x00,
            0x00, 0x3a, 0x10, 0x58, 0x2d, 0x54, 0x69, 0x6d,
            0x65, 0x73, 0x74, 0x61, 0x6d, 0x70, 0x2d, 0x42,
            0x61, 0x73, 0x65, 0x53, 0x00, 0x00, 0x00, 0x0b,
            0x31, 0x32, 0x30, 0x31, 0x32, 0x36, 0x36, 0x39,
            0x30, 0x31, 0x00, 0x11, 0x58, 0x2d, 0x54, 0x69,
            0x6d, 0x65, 0x73, 0x74, 0x61, 0x6d, 0x70, 0x2d,
            0x44, 0x65, 0x6c, 0x74, 0x61, 0x53, 0x00, 0x00,
            0x00, 0x02, 0x30, 0x00, 0xce};
        decoder.write (payload, 77);
    }

    {
        unsigned char header [] = {0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06};
        decoder.write (header, 7);

        unsigned char payload [] = {0x20, 0x26, 0x45, 0x95, 0xa9, 0x07, 0xce};
        decoder.write (payload, 7);
    }
*/
    return 0;
}

