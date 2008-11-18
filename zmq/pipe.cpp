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

#include "pipe.hpp"
#include "command.hpp"

zmq::pipe_t::pipe_t (i_thread *source_thread_, i_engine *source_engine_,
      i_thread *destination_thread_, i_engine *destination_engine_) :
    pipe (false),
    source_thread (source_thread_),
    source_engine (source_engine_),
    destination_thread (destination_thread_),
    destination_engine (destination_engine_),
    alive (true),
    writer_terminating (false),
    reader_terminating (false)
{
}

zmq::pipe_t::~pipe_t ()
{
    //  Destroy the messages in the pipe itself.
    raw_message_t message;
    pipe.flush ();
    while (pipe.read (&message))
        raw_message_destroy (&message);
}

void zmq::pipe_t::revive ()
{
    assert (!alive);
    alive = true;
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
    //  there's only a delimiter in the pipe, which makes memory usage very low.
    //  Moreover, even reading from different pipes will make the CFQ mechanism
    //  roll over these unterminated pipes and terminate them.
    if (msg_->content == (void*) raw_message_t::delimiter_tag) {
        terminate_reader ();
        return false;
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
