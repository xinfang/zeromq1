/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_AMQP09_ENGINE_HPP_INCLUDED__
#define __ZMQ_AMQP09_ENGINE_HPP_INCLUDED__

#include "i_pollable.hpp"
#include "i_signaler.hpp"
#include "dispatcher.hpp"
#include "amqp09_encoder.hpp"
#include "amqp09_decoder.hpp"
#include "amqp09_unmarshaller.hpp"
#include "tcp_socket.hpp"

#include <sys/poll.h>

namespace zmq
{

    //  amqp09_engine uses TCP to transport messages using AMQP version 0-9.
    //  Event handling is done via poll - i.e. the engine should be used with
    //  poll_thread.
    //
    //  As AMQP defines two different state machines (server & client)
    //  amqp09_engine is a template parameterised by the state machine
    //  implementation (either amqp09_server_fsm or amqp09_client_fsm).

    template <typename F> class amqp09_engine_t :
        public i_pollable, private i_signaler
    {
    public:

        typedef F fsm_t;

        //  Creates amqp09_engine. Attaches it to dispatcher using engine_id
        //  supplied. Underlying TCP is initialised using listen, address
        //  and port parameters. source_engine_id specifies which engine
        //  to get messages from to be send to the socket, destination_engine_id
        //  specified which engine to send incoming messages to. writebuf_size
        //  and readbuf_size determine the amount of batching to use.
        //  out_exchange and out_routing_key are used to set exchange and
        //  routing key on outgoing messages. in_exchange and on in_routing_key
        //  are used to subscribe for incoming messages.
        amqp09_engine_t (dispatcher_t *dispatcher_, int engine_id_,
              bool listen_, const char *address_, uint16_t port_,
              int source_engine_id_, int destination_engine_id_,
              size_t writebuf_size_, size_t readbuf_size_,
              const char *out_exchange_, const char *out_routing_key_,
              const char *in_exchange_, const char *in_routing_key_) :
            socket (listen_, address_, port_),
            proxy (dispatcher_, engine_id_),
            marshaller (this),
            fsm (&socket, &marshaller, this, in_exchange_, in_routing_key_),
            unmarshaller (&fsm),
            encoder (&proxy, source_engine_id_, &marshaller, fsm.server (),
                out_exchange_, out_routing_key_),
            decoder (&proxy, destination_engine_id_, &unmarshaller,
                fsm.server ()),
            writebuf_size (writebuf_size_),
            readbuf_size (readbuf_size_),
            write_size (0),
            write_pos (0),
            source_engine_id (source_engine_id_),
            destination_engine_id (destination_engine_id_),
            events (POLLIN | POLLOUT)
        {
            writebuf = (unsigned char*) malloc (writebuf_size);
            assert (writebuf);
            readbuf = (unsigned char*) malloc (readbuf_size);
            assert (readbuf);
        }

        ~amqp09_engine_t ()
        {
            free (readbuf);
            free (writebuf);
        }

        // i_pollable interface implementation

        inline void set_signaler (i_signaler *signaler_)
        {
            proxy.set_signaler (signaler_);
        }

        inline void revive (int engine_id_)
        {
            proxy.revive (engine_id_);
            events |= POLLOUT;
        }

        inline int get_fd ()
        {
            return socket.get_fd ();
        }

        inline short get_events ()
        {
            return events | (proxy.get_self_signal () ? POLLOUT : 0);
        }

        void in_event ()
        {
            //  Get as much data from the socket as possible
            size_t nbytes = socket.read (readbuf, readbuf_size);

            //  If the socket was closed by the other party, stop polling for in
            if (!nbytes) {
                events ^= POLLIN;
                return;
            }

            //  Push the data to the encoder
            decoder.write (readbuf, nbytes);

            //  Flush any messages decoder may have produced to the dispatcher
            proxy.flush ();
        }

        void out_event ()
        {
            //  If there are no more data to write, try to get more from
            //  the encoder. If none are available, stop polling for out.
            if (write_pos == write_size) {

                write_size = encoder.read (writebuf, writebuf_size);
                write_pos = 0;

                if (!write_size)
                     events ^= POLLOUT;
            }

            //  Write as much of the data to the socket as possible
            if (write_pos < write_size) {
                size_t nbytes = socket.write (writebuf + write_pos,
                    write_size - write_pos);
                write_pos += nbytes;
            }
        }

        //  Interface to communicate with embedded marshaller object
        //  TODO: the interface should be the engine class itself rather
        //  than i_signaler... give it a thought.

        inline void signal (int signal_)
        {
            events |= POLLOUT;
        }

        //  Interface to communicate with embedded state machine object

        inline void flow (bool on_)
        {
            encoder.flow (on_);
            decoder.flow (on_);

            if (on_) {
                events |= POLLIN;
                events |= POLLOUT;
            }
        }

    private:

        tcp_socket_t socket;
        dispatcher_proxy_t proxy;
        amqp09_marshaller_t marshaller;
        fsm_t fsm;
        amqp09_unmarshaller_t unmarshaller;
        amqp09_encoder_t encoder;
        amqp09_decoder_t decoder;

        short events;

        unsigned char *writebuf;
        size_t writebuf_size;

        unsigned char *readbuf;
        size_t readbuf_size;

        size_t write_size;
        size_t write_pos;

        int source_engine_id;
        int destination_engine_id;
    };

}

#endif
