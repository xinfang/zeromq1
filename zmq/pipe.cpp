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

#include <limits>
#include "pipe.hpp"
#include "command.hpp"
#include "mux.hpp"
#include "config.hpp"
#include "atomic_counter.hpp"

zmq::pipe_t::pipe_t (i_context *source_context_, i_engine *source_engine_,
      i_context *destination_context_, i_engine *destination_engine_,
      int hwm_, int lwm_, const char *queue_name_, uint64_t swap_size_) :
    pipe (false),
    source_context (source_context_),
    source_engine (source_engine_),
    destination_context (destination_context_),
    destination_engine (destination_engine_),
    mux (NULL),
    alive (true), 
    endofpipe (false),
    hwm (hwm_),
    lwm (lwm_),
    size_monitoring (false),
    write_msg_cnt (0),
    read_msg_cnt (0),
    head (0),
    tail (0),
    last_head (0),
    swap (NULL),
    swapping (false),
    in_memory_tail (0)
{
    //  If high water mark was lower than low water mark pipe would hang up.
    assert (lwm <= hwm);

    //  If size monitoring is required, store the queue name.
    if (queue_name_) {
        size_monitoring = true;
        queue_name = queue_name_;
    }

    //  If swapping is enabled, create the swap file with an unique name.
    if (swap_size_) {

        //  TODO: Does this variable need to by made thread-safe?
        static int counter = 1;

        char buff [256];
        snprintf (buff, 256, "%s.%d.dat", queue_name.c_str (), counter ++);
        swap = new msg_store_t (buff, swap_size_);
        assert (swap);
    }
}

zmq::pipe_t::~pipe_t ()
{
    //  Adjust the queue size statistics.
    if (size_monitoring) {
        int delta = head % queue_size_granularity;
        if (delta != 0)
            source_context->adjust_queue_size (queue_name.c_str (), -delta);
    }

    //  Destroy the swap file.
    if (swap)
        delete swap;

    //  Destroy the messages in the pipe itself.
    raw_message_t message;
    pipe.flush ();
    while (pipe.read (&message))
        raw_message_destroy (&message);
}

void zmq::pipe_t::set_mux (mux_t *mux_)
{
    assert (!mux);
    mux = mux_;
}

bool zmq::pipe_t::check_write ()
{
    if (swapping)
        return true;

    if (hwm) {

        //  If pipe size have reached high watermark, reject the write.
        //  The else branch will come into effect after 500,000 years of
        //  passing 1,000,000 messages a second but it's implemented just
        //  in case ...
        int size = last_head <= tail ? tail - last_head :
            std::numeric_limits <uint64_t>::max () - last_head + tail + 1;
     
        if (size == hwm)
            if (swap) {
                swapping = true;
                in_memory_tail = tail;
            }
            else
                return false;
    }
    return true;
}

void zmq::pipe_t::write (raw_message_t *msg_)
{
    //  Write the message either to the pipe or to the swap.
    if (swapping) {

        //  If the space in the swap file was used up, abort the application.
        if (!swap->store (msg_))
            assert (false);
    }
    else {
        pipe.write (*msg_);
        in_memory_tail ++;
    }

    //  Move the tail.
    tail ++;

    if (msg_->content == (void*) raw_message_t::delimiter_tag)
        return;

    //  Adjust the queue size statistics once in a while.
    if (size_monitoring) {
        if (++ write_msg_cnt == queue_size_granularity) {
            source_context->adjust_queue_size (queue_name.c_str (),
                write_msg_cnt);
            write_msg_cnt = 0;
        }
    }
}

void zmq::pipe_t::write_delimiter ()
{
    //  Adjust queue size.
    if (write_msg_cnt > 0) {
        source_context->adjust_queue_size (queue_name.c_str (),
            write_msg_cnt);
        write_msg_cnt = 0;
    }

    //  Write delimiter into pipe.
    raw_message_t delimiter;
    raw_message_init_delimiter (&delimiter);
    write (&delimiter);
    flush ();
}

void zmq::pipe_t::flush ()
{
    if (!pipe.flush ()) {
        command_t cmd;
        cmd.init_engine_revive (destination_engine, this);
        source_context->send_command (destination_context, cmd);
    }
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

    if (swapping && in_memory_tail - last_head <= (uint64_t) lwm) {

         //  Now we have to re-fill the pipe using data from the swap.
         //  Once the swap is empty we'll switch into non-swapping mode.
         uint64_t refill_count = hwm - (in_memory_tail - last_head);
         for (uint64_t i = 0; i != refill_count; i ++) {
             if (swap->empty ()) {
                 assert (tail == in_memory_tail);
                 swapping = false;
                 break;
             }

             raw_message_t msg;
             swap->fetch (&msg);
             pipe.write (msg);
             in_memory_tail ++;       
         }

         //  Flush all the messages into the pipe in a single go.
         flush ();
    }
}

bool zmq::pipe_t::read (raw_message_t *msg_)
{
    //  If the pipe is dead, there's nothing we can do.
    if (!alive)
        return false;

    do {
        //  Get next message, if it's not there, die.
        if (!pipe.read (msg_)) {
            alive = false;
            return false;
        }

        //  Adjust head position.
        head ++;

        //  Once in N messages send current head position to the writer thread.
        if (hwm) {

            //  If high water mark is same as low water mark we have to report each
            //  message retrieval from the pipe. Otherwise the period N is computed
            //  as a difference between high and low water marks.
            if (head % (hwm - lwm + 1) == 0) {
                command_t cmd;
                cmd.init_engine_head (source_engine, this, head);
                destination_context->send_command (source_context, cmd);
            }
        }

        if (msg_->content == (void*) raw_message_t::delimiter_tag) {
            if (read_msg_cnt > 0) {
                source_context->adjust_queue_size (queue_name.c_str (),
                    -read_msg_cnt);
                read_msg_cnt = 0;
            }
        }
    }
    while (msg_->content == (void*) raw_message_t::delimiter_tag);

    //  Adjust the queue size statistics once in a while.
    if (size_monitoring) {
        if (++ read_msg_cnt == queue_size_granularity) {
            source_context->adjust_queue_size (queue_name.c_str (),
                -read_msg_cnt);
            read_msg_cnt = 0;
        }
    }

    return true;
}

void zmq::pipe_t::send_destroy_pipe ()
{
    command_t cmd;
    cmd.init_engine_destroy_pipe (source_engine, this);
    destination_context->send_command (source_context, cmd);
}
