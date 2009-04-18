/*
    Copyright (c) 2007-2009 FastMQ Inc.

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

#include <zmq/pipe.hpp>
#include <zmq/command.hpp>
#include <zmq/err.hpp>

zmq::pipe_t::pipe_t (i_thread *source_thread_, i_engine *source_engine_,
      i_thread *destination_thread_, i_engine *destination_engine_) :
    pipe (false),
    source_thread (source_thread_),
    source_engine (source_engine_),
    destination_thread (destination_thread_),
    destination_engine (destination_engine_),
    mux_index (0),
    head (0),
    last_head_position (0),
    delayed_gap (false),
    in_core_msg_cnt (0),
    swap (NULL),
    swapping (false),
    in_swap_msg_cnt (0),
    writer_terminating (false),
    reader_terminating (false)
{
    //  Compute watermarks for the pipe. If either of engines has infinite
    //  watermarks (hwm = 0) the pipe watermarks will be infinite as well.
    //  Otherwise pipe watermarks are sum of exchange and queue watermarks.
    int64_t shwm;
    int64_t slwm;
    source_engine->get_watermarks (&shwm, &slwm);
    int64_t dhwm;
    int64_t dlwm;
    destination_engine->get_watermarks (&dhwm, &dlwm);
    if (shwm == -1 || dhwm == -1) {
        hwm = 0;
        lwm = 0;
    }
    else {
        hwm = shwm + dhwm;
        lwm = slwm + dlwm;
    }

    int64_t swap_size = source_engine->get_swap_size () +
        destination_engine->get_swap_size ();

    //  Create a swap file if necessary.
    if (swap_size > 0) {
        swap = new swap_t (swap_size);
        zmq_assert (swap);
    }
}

zmq::pipe_t::~pipe_t ()
{
    //  Purge the associated swap.
    if (swap)
        delete swap;

    //  Destroy the messages in the pipe itself.
    raw_message_t message;
    pipe.flush ();
    while (pipe.read (&message))
        raw_message_destroy (&message);
}

bool zmq::pipe_t::check_write ()
{
    return (hwm == 0 || in_core_msg_cnt < (size_t) hwm || swap);
}


void zmq::pipe_t::write (raw_message_t *msg_)
{
    //  If we are allowed to write to the pipe, delayed gap notification must
    //  have been written beforehand.
    zmq_assert (!delayed_gap);

    //  If we have hit the queue limit, switch into swapping mode.
    if (in_core_msg_cnt == (size_t) hwm && hwm != 0) {
        zmq_assert (swap);
        swapping = true;
    }

    //  Write the message into main memory or swap file.
    if (swapping) {
        bool rc = swap->store (msg_);
        zmq_assert (rc);
        in_swap_msg_cnt ++;
    }
    else {
        pipe.write (*msg_);
        in_core_msg_cnt ++;
    }
}

void zmq::pipe_t::gap ()
{
    if (!check_write ()) {
        delayed_gap = true;
        return;
    }
    raw_message_t msg;
    raw_message_init_notification (&msg, raw_message_t::gap_tag);
    write (&msg);
    flush ();
}

void zmq::pipe_t::set_head (uint64_t position_)
{
    //  This may cause the next write to succeed.
    in_core_msg_cnt -= position_ - last_head_position;
    last_head_position = position_;

    //  Transfer messages from the swap into the main memory.
    if (swapping && in_core_msg_cnt < (size_t) lwm)
        swap_in ();

    //  If there's a gap notification waiting, push it into the queue.
    if (delayed_gap && check_write ()) {
        raw_message_t msg;
        raw_message_init_notification (&msg, raw_message_t::gap_tag);
        write (&msg);
        delayed_gap = false;
    }
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
    //  Get next message, if there's none, die.
    if (!pipe.read (msg_))
        return false;

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
        raw_message_init_notification (&delimiter,
            raw_message_t::delimiter_tag);
        pipe.write (delimiter);
        flush ();
        writer_terminating = true;
    }
}

void zmq::pipe_t::writer_terminated ()
{
    i_thread *st = source_thread;

    //  Drop the pointers to the writer. This has no real effect and is even
    //  incorrect w.r.t. CPU cache coherency rules, however, it may cause 0MQ
    //  to fail faster in case of certain synchronisation bugs.
    source_thread = NULL;
    source_engine = NULL;

    //  Send termination acknowledgement to the pipe reader.
    command_t cmd;
    cmd.init_engine_terminate_pipe_ack (destination_engine, this);
    st->send_command (destination_thread, cmd);
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

void zmq::pipe_t::swap_in ()
{
    while (in_swap_msg_cnt > 0 && in_core_msg_cnt < (size_t) hwm) {
        raw_message_t msg;
        swap->fetch (&msg);
        pipe.write (msg);
        in_swap_msg_cnt --;
        in_core_msg_cnt ++;
    }

    //  Flush all messages.
    flush ();

    if (in_swap_msg_cnt == 0) {
        zmq_assert (swap->empty ());
        swapping = false;
    }
}
