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

#include "pipe.hpp"
#include "command.hpp"

zmq::pipe_t::pipe_t (i_context *source_context_, i_engine *source_engine_,
      i_context *destination_context_, i_engine *destination_engine_) :
    pipe (false),
    source_context (source_context_),
    source_engine (source_engine_),
    destination_context (destination_context_),
    destination_engine (destination_engine_),
    alive (true), 
    endofpipe (false)
{
}

zmq::pipe_t::~pipe_t ()
{
    //  Flush so that we can read all the messages from the pipe
    pipe.flush ();

    //  Destroy the messages in the pipe itself
    pipe_element_t element;
    while (pipe.read (&element))
        if (element.msg != NULL &&
              element.msg != (void*) pipe_element_t::vsm_tag)
            msg_dealloc (element.msg);
}

void zmq::pipe_t::revive ()
{
    assert (!alive);
    alive = true;
}

void zmq::pipe_t::send_revive ()
{
    command_t cmd;
    cmd.init_engine_revive (destination_engine, this);
    source_context->send_command (destination_context, cmd);
}

void *zmq::pipe_t::read ()
{
    //  If the pipe is dead, there's nothing we can do
    if (!alive)
        return NULL;

    //  Get next message, if it's not there, die.
    pipe_element_t element;
    if (!pipe.read (&element))
    {
        alive = false;
        return NULL;
    }

    //  if message is stored in the element's buffer,
    //  copy it to full-blown message.
    if (element.msg == (void*) pipe_element_t::vsm_tag) {
        void *msg = msg_alloc (element.vsm_size);
        assert (msg);
        memcpy (msg_data (msg), element.vsm_data, element.vsm_size);
        return msg;
    }

    //  If delimiter is read from the pipe, mark the pipe as ended
    if (!element.msg)
        endofpipe = true;

    return element.msg;
}

void zmq::pipe_t::send_destroy_pipe ()
{
    command_t cmd;
    cmd.init_engine_destroy_pipe (source_engine, this);
    destination_context->send_command (source_context, cmd);
}

