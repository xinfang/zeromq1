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

#ifndef __ZMQ_AMQP_ENCODER_HPP_INCLUDED__
#define __ZMQ_AMQP_ENCODER_HPP_INCLUDED__

#if defined ZMQ_HAVE_AMQP

#include <string>

#include <zmq/encoder.hpp>
#include <zmq/i_amqp.hpp>
#include <zmq/amqp_marshaller.hpp>

namespace zmq
{

    //  Encoder for AMQP.
    class amqp_encoder_t : public encoder_t <amqp_encoder_t>
    {
    public:

        amqp_encoder_t (bool server_);
        ~amqp_encoder_t ();

    private:

        bool message_ready ();
        bool command_header ();
        bool command_arguments ();
        bool content_header ();
        bool content_body_frame_header ();
        bool content_body ();
        bool frame_end ();

        //  If true, this is 'server' side of the AMQP connection.
        //  Messages are passed using basic.deliver command. If false,
        //  it's client side of connection, messages are passed using
        //  basic.publish.
        bool server;

        //  Object encoding & storing commands to send. If there are any
        //  commands stored in the
        amqp_marshaller_t marshaller;
    };

}

#endif

#endif

