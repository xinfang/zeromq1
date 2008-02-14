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

#ifndef __ZMQ_AMQP09_ENCODER_HPP_INCLUDED__
#define __ZMQ_AMQP09_ENCODER_HPP_INCLUDED__

#include "encoder.hpp"
#include "dispatcher_proxy.hpp"
#include "amqp09_marshaller.hpp"

namespace zmq
{

    class amqp09_encoder_t : public encoder_t <amqp09_encoder_t>
    {
    public:

        amqp09_encoder_t (dispatcher_proxy_t *proxy_, int source_thread_id_,
            amqp09_marshaller_t *marshaller_);
        ~amqp09_encoder_t ();

        void flow (bool on_);

    private:

        bool message_ready ();
        bool command_header ();
        bool command_arguments ();
        bool content_header ();
        bool content_body_frame_header ();
        bool content_body ();
        bool frame_end ();

        unsigned char *tmpbuf;
        size_t tmpbuf_size;

        amqp09_marshaller_t::command_t command;
        cmsg_t message;
        size_t body_offset;

        dispatcher_proxy_t *proxy;
        int source_thread_id;
        amqp09_marshaller_t *marshaller;

        bool flow_on;
    };

}

#endif
