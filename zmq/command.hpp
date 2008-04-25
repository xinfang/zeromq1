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

#ifndef __ZMQ_COMMAND_HPP_INCLUDED__
#define __ZMQ_COMMAND_HPP_INCLUDED__

#include "i_pollable.hpp"
#include "ysemaphore.hpp"
#include "pipe.hpp"

namespace zmq
{

    struct engine_command_t
    {
        enum type_t
        {
            revive,
            send_to,
            receive_from
        } type;

        union {
            struct {
                class pipe_t *pipe;
            } revive;
            struct {
                class pipe_t *pipe;
            } send_to;
            struct {
                class pipe_t *pipe;
            } receive_from;
        } args;   
    };

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
                struct i_pollable *engine;
                ysemaphore_t *blocker;
            } register_engine;
            struct {
                struct i_pollable *engine;
                ysemaphore_t *blocker;
            } unregister_engine;
            struct {
                struct i_pollable *engine;
                engine_command_t command;
            } engine_command;
        } args;

        inline void init_stop ()
        {
            type = stop;
        }

        inline void init_register_engine (i_pollable *engine_)
        {
            type = register_engine;
            args.register_engine.engine = engine_;
        }

        inline void init_unregister_engine (i_pollable *engine_)
        {
            type = unregister_engine;
            args.unregister_engine.engine = engine_;
        }

        inline void init_engine_send_to (i_pollable *engine_, pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::send_to;
            args.engine_command.command.args.send_to.pipe = pipe_;
        }

        inline void init_engine_receive_from (i_pollable *engine_,
            pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::receive_from;
            args.engine_command.command.args.receive_from.pipe = pipe_;
        }

        inline void init_engine_revive (i_pollable *engine_,
            pipe_t *pipe_)
        {
            type = engine_command;
            args.engine_command.engine = engine_;
            args.engine_command.command.type = engine_command_t::revive;
            args.engine_command.command.args.revive.pipe = pipe_;
        }

    };

}    

#endif
