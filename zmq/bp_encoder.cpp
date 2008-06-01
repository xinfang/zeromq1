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

#include "bp_encoder.hpp"
#include "wire.hpp"

zmq::bp_encoder_t::bp_encoder_t (mux_t *mux_) :
    mux (mux_)
{
    cmsg.msg = NULL;
    next_step (NULL, 0, &bp_encoder_t::message_ready);
}

zmq::bp_encoder_t::~bp_encoder_t ()
{
    msg_dealloc (cmsg.msg);
}

bool zmq::bp_encoder_t::size_ready ()
{
    //  Write message content
    next_step (msg_data (cmsg.msg), msg_size (cmsg.msg),
        &bp_encoder_t::message_ready);
    return true;
}

bool zmq::bp_encoder_t::message_ready ()
{
    //  Get rid of the old message
    msg_dealloc (cmsg.msg);
    cmsg.msg = NULL;

    //  Read new message from mux, if there is none, return false.
    if (!mux->read (&cmsg))
        return false;

    if (msg_size (cmsg.msg) < 255) {

        //  Write one-byte length
        tmpbuf [0] = (unsigned char) msg_size (cmsg.msg);
        next_step (tmpbuf, 1, &bp_encoder_t::size_ready);
    }
    else {

        //  Write 0xff escape character & 8-byte length
        tmpbuf [0] = 0xff;
        put_uint64 (tmpbuf + 1, msg_size (cmsg.msg));
        next_step (tmpbuf, 9, &bp_encoder_t::size_ready);
    }
    return true;
}
