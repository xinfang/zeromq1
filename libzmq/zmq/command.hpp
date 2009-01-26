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

#include <assert.h>
#include <string.h>

#include <zmq/stdint.hpp>
#include <zmq/i_engine.hpp>
#include <zmq/pipe.hpp>
#include <zmq/formatting.hpp>

namespace zmq
{

    //  This structure defines all the commands that can be sent to an engine.

    struct engine_command_t
    {
        enum type_t
        {
            revive,
            head,
            send_to,
            receive_from,
            terminate_pipe,
            terminate_pipe_ack
        } type;

        union {
            struct {
                class pipe_t *pipe;
            } revive;
            struct {
                class pipe_t *pipe;
                uint64_t position;
            } head;
            struct {
                class pipe_t *pipe;
            } send_to;
            struct {
                class pipe_t *pipe;
            } receive_from;
            struct {
                class pipe_t *pipe;
            } terminate_pipe;
            struct {
                class pipe_t *pipe;
            } terminate_pipe_ack;
        } args;   
    };

    //  This structure defines all the commands that can be sent to a thread.
    //  It also provides 'constructors' for all the commands.

    struct command_t
    {
        enum type_t
        {
            stop,
            register_engine,
            unregister_engine,
            engine_command
        } type;

        union
        {
            struct {
            } stop;
            struct {
                i_engine *engine;
            } register_engine;
            struct {
                i_engine *engine;
            } unregister_engine;
            struct {
                i_engine *engine;
                engine_command_t command;
            } engine_command;
        } args;

        inline void init_stop ()
        {
            type = stop;
        }

        inline void init_register_engine (i_engine *engine_)
        {
            type = register_engine;
            args.register_engine.engine = engine_;
        }

        inline void init_unregister_engine (i_engine *engine_)
        {
            type = unregister_engine;
            args.unregister_engine.engine = engine_;
        }

        inline void init_engine_send_to (i_engine *engine_, pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::send_to;
            args.engine_command.command.args.send_to.pipe = pipe_;
        }

        inline void init_engine_receive_from (i_engine *engine_, pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::receive_from;
            args.engine_command.command.args.receive_from.pipe = pipe_;
        }

        inline void init_engine_revive (i_engine *engine_,
            pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::revive;
            args.engine_command.command.args.revive.pipe = pipe_;
        }

        inline void init_engine_head (i_engine *engine_, pipe_t *pipe_,
            uint64_t position_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::head;
            args.engine_command.command.args.head.pipe = pipe_;
            args.engine_command.command.args.head.position = position_;
        }

        inline void init_engine_terminate_pipe (i_engine *engine_,
            pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::terminate_pipe;
            args.engine_command.command.args.terminate_pipe.pipe = pipe_;
        }

        inline void init_engine_terminate_pipe_ack (i_engine *engine_,
            pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type =
                engine_command_t::terminate_pipe_ack;
            args.engine_command.command.args.terminate_pipe_ack.pipe = pipe_;
        }
        
    };

}    

#endif
