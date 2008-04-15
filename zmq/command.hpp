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

namespace zmq
{

    struct engine_command_t
    {
        enum type_t
        {
            revive
        } type;

        union {
            struct {
            } revive;
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
                i_pollable *engine;
                ysemaphore_t *blocker;
            } register_engine;
            struct {
                i_pollable *engine;
                ysemaphore_t *blocker;
            } unregister_engine;
            struct {
                i_pollable *engine;
                engine_command_t command;
            } engine_command;
        } args;
    };

}    

#endif
