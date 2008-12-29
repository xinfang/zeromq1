/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <limits>

#include <zmq/pipe.hpp>
#include <zmq/command.hpp>

zmq::pipe_t::pipe_t (i_thread *source_thread_, i_engine *source_engine_,
      i_thread *destination_thread_, i_engine *destination_engine_) :
    pipe (false),
    source_thread (source_thread_),
    source_engine (source_engine_),
    destination_thread (destination_thread_),
    destination_engine (destination_engine_),
    alive (true),
    head (0),
    tail (0),
    last_head (0),
    writer_terminating (false),
    reader_terminating (false)
{
    //  Compute watermarks for the pipe. If either of engines has infinite
    //  watermarks (hwm = 0) the pipe watermarks will be infinite as well.
    //  Otherwise pipe watermarks are sum of exchange and queue watermarks.
    uint64_t shwm;
    uint64_t slwm;
    source_engine->get_watermarks (&shwm, &slwm);
    uint64_t dhwm;
    uint64_t dlwm;
    destination_engine->get_watermarks (&dhwm, &dlwm);
    if (!shwm || !dhwm) {
        hwm = 0;
        lwm = 0;
    }
    else {
        hwm = shwm + dhwm;
        lwm = slwm + dlwm;
    }
}

zmq::pipe_t::~pipe_t ()
{
    //  Destroy the messages in the pipe itself.
    raw_message_t message;
    pipe.flush ();
    while (pipe.read (&message))
        raw_message_destroy (&message);
}

bool zmq::pipe_t::check_write ()
{
    if (!hwm)
        return true;

    //  If pipe size have reached high watermark, reject the write.
    //  The else branch will come into effect after 500,000 years of
    //  passing 1,000,000 messages a second but still, it's implemented just
    //  in case ...
    int size = last_head <= tail ? tail - last_head :
        std::numeric_limits <uint64_t>::max () - last_head + tail + 1;
    assert (size <= hwm);
     
    return size < hwm;
}

void zmq::pipe_t::write (raw_message_t *msg_)
{
    //  Physically write the message to the pipe.
    pipe.write (*msg_);

    //  Move the tail.
    tail ++;
}

void zmq::pipe_t::revive ()
{
    assert (!alive);
    alive = true;
}

void zmq::pipe_t::set_head (uint64_t position_)
{
    //  This may cause the next write to succeed.
    last_head = position_;
}

void zmq::pipe_t::flush ()
{
    if (!pipe.flush ()) {
        command_t cmd;
        cmd.init_engine_revive (destination_engine, this);
        source_thread->send_command (destination_thread, cmd);
    }
}

bool zmq::pipe_t::read (raw_message_t *msg_)
{
    //  If the pipe is dead, there's nothing we can do.
    if (!alive)
        return false;

    //  Get next message, if it's not there, die.
    if (!pipe.read (msg_))
    {
        alive = false;
        return false;
    }

    //  If delimiter is read from the pipe, start the shutdown process.
    //  If 'read' is not called the pipe would hang in the memory for
    //  an indefinite amount of time. However: If there are messages in the
    //  pipe, the pipe cannot be dropped anyway. Thus the only problem is when
    //  there's only a delimiter in the pipe, which makes memory overhead quite
    //  low. Moreover, reading even from different pipes will make
    //  the CFQ mechanism roll over these unterminated pipes and terminate them.
    if (msg_->content == (void*) raw_message_t::delimiter_tag) {
        terminate_reader ();
        return false;
    }

    //  Once in N messages send current head position to the writer thread.
    if (hwm) {
        head ++;

        //  If high water mark is same as low water mark we have to report each
        //  message retrieval from the pipe. Otherwise the period N is computed
        //  as a difference between high and low water marks.
        if (head % (hwm - lwm + 1) == 0) {
            command_t cmd;
            cmd.init_engine_head (source_engine, this, head);
            destination_thread->send_command (source_thread, cmd);
        }
    }

    return true;
}

void zmq::pipe_t::terminate_writer ()
{
    if (!writer_terminating) {

        //  Push the delimiter to the pipe. Delimiter is a message for pipe
        //  reader that there will be no more messages in the pipe.
        raw_message_t delimiter;
        raw_message_init_delimiter (&delimiter);
        pipe.write (delimiter);
        flush ();
        writer_terminating = true;
    }
}

void zmq::pipe_t::writer_terminated ()
{
    //  Send termination acknowledgement to the pipe reader.
    command_t cmd;
    cmd.init_engine_terminate_pipe_ack (destination_engine, this);
    source_thread->send_command (destination_thread, cmd);

    //  Drop the pointers to the writer. This has no real effect and is even
    //  incorrect w.r.t. CPU cache coherency rules, however, it may cause 0MQ
    //  to fail faster in case of certain synchronisation bugs.
    source_thread = NULL;
    source_engine = NULL;
}

void zmq::pipe_t::terminate_reader ()
{
    if (!reader_terminating) {

        //  Send termination request to the pipe writer.
        command_t cmd;
        cmd.init_engine_terminate_pipe (source_engine, this);
        destination_thread->send_command (source_thread, cmd);
        reader_terminating = true;
    }
}

void zmq::pipe_t::reader_terminated ()
{
    //  Drop the pointers to the reader. This has no real effect and is even
    //  incorrect w.r.t. CPU cache coherency rules, however, it may cause 0MQ
    //  to fail faster in case of certain synchronisation bugs.
    destination_thread = NULL;
    destination_engine = NULL;
}
