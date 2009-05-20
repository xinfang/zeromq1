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

#ifndef __ZMQ_COMMAND_HPP_INCLUDED__
#define __ZMQ_COMMAND_HPP_INCLUDED__

#include <string.h>

#include <zmq/stdint.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/pipe.hpp>
#include <zmq/formatting.hpp>

namespace zmq
{

    //  This structure defines all the commands that can be sent to a thread.
    //  It also provides 'constructors' for all the commands.

    struct command_t
    {
        enum type_t
        {
            stop,
            attach_pipe_to_demux,
            attach_pipe_to_mux,
            revive_reader,
            notify_writer,
            terminate_pipe_req,
            terminate_pipe_ack,
            register_pollable,
            unregister_pollable,
            engine_command
        } type;

        union
        {
            struct {
            } stop;
            struct {
                class pipe_t *pipe;
                class i_demux *demux;
            } attach_pipe_to_demux;
            struct {
                class pipe_t *pipe;
                class i_mux *mux;
            } attach_pipe_to_mux;
            struct {
                class pipe_t *pipe;
            } revive_reader;
            struct {
                class pipe_t *pipe;
                uint64_t position;
            } notify_writer;
            struct {
                class pipe_t *pipe;
            } terminate_pipe_req;
            struct {
                class pipe_t *pipe;
            } terminate_pipe_ack;
            struct {
                i_pollable *pollable;
            } register_pollable;
            struct {
                i_pollable *pollable;
            } unregister_pollable;
        } args;

        inline void init_stop ()
        {
            type = stop;
        }

        inline void init_attach_pipe_to_demux (class i_demux *demux_,
            class pipe_t *pipe_)
        {
            type = attach_pipe_to_demux;
            args.attach_pipe_to_demux.pipe = pipe_;
            args.attach_pipe_to_demux.demux = demux_;
        }

        inline void init_attach_pipe_to_mux (class i_mux *mux_,
            class pipe_t *pipe_)
        {
            type = attach_pipe_to_mux;
            args.attach_pipe_to_mux.pipe = pipe_;
            args.attach_pipe_to_mux.mux = mux_;
        }

        inline void init_revive_reader (class pipe_t *pipe_)
        {
            type = revive_reader;
            args.revive_reader.pipe = pipe_;
        }

        inline void init_notify_writer (class pipe_t *pipe_, uint64_t position_)
        {
            type = notify_writer;
            args.notify_writer.pipe = pipe_;
            args.notify_writer.position = position_;
        }

        inline void init_terminate_pipe_req (class pipe_t *pipe_)
        {
            type = terminate_pipe_req;
            args.terminate_pipe_req.pipe = pipe_;
        }

        inline void init_terminate_pipe_ack (class pipe_t *pipe_)
        {
            type = terminate_pipe_ack;
            args.terminate_pipe_ack.pipe = pipe_;
        }

        inline void init_register_pollable (i_pollable *pollable_)
        {
            type = register_pollable;
            args.register_pollable.pollable = pollable_;
        }

        inline void init_unregister_pollable (i_pollable *pollable_)
        {
            type = unregister_pollable;
            args.unregister_pollable.pollable = pollable_;
        }

    };

}    

#endif
