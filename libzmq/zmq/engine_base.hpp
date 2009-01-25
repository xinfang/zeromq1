/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHHAS_OUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_ENGINE_BASE_HPP_INCLUDED__
#define __ZMQ_ENGINE_BASE_HPP_INCLUDED__

#include <assert.h>

#include <zmq/i_engine.hpp>
#include <zmq/mux.hpp>
#include <zmq/demux.hpp>

namespace zmq
{

    template <bool HAS_IN, bool HAS_OUT> class engine_base_t : public i_engine
    {
    protected:

        i_pollable *cast_to_pollable ()
        {
            assert (false);
            return NULL;
        }

        const char *get_arguments ()
        {
            assert (false);
            return NULL;
        }

        void revive (pipe_t *pipe_)
        {
            //  Notify the reader of the pipe that there are messages
            //  available in the pipe.
            assert (HAS_OUT);
            pipe_->revive ();
        }

        void head (pipe_t *pipe_, uint64_t position_)
        {
            //  Forward pipe head position to the appropriate pipe.
            assert (HAS_IN);
            pipe_->set_head (position_);
        }

        void send_to (pipe_t *pipe_)
        {
            //  Start sending messages to a pipe.
            assert (HAS_IN);
            demux.send_to (pipe_);
        }

        void receive_from (pipe_t *pipe_)
        {
            //  Start receiving messages from a pipe.
            assert (HAS_OUT);
            mux.receive_from (pipe_);
        }

        void terminate_pipe (pipe_t *pipe_)
        {
            //  Forward the command to the pipe. Drop reference to the pipe.
            assert (HAS_IN);
            pipe_->writer_terminated ();
            demux.release_pipe (pipe_);
        }

        void terminate_pipe_ack (pipe_t *pipe_)
        {
            //  Forward the command to the pipe. Drop reference to the pipe.
            assert (HAS_OUT);
            pipe_->reader_terminated ();
            mux.release_pipe (pipe_);
        }

        mux_t mux;
        demux_t demux;
    };

}

#endif
