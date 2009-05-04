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

#include <zmq/err.hpp>
#include <zmq/i_engine.hpp>
#include <zmq/mux.hpp>
#include <zmq/i_demux.hpp>
#include <zmq/data_distributor.hpp>
#include <zmq/load_balancer.hpp>

namespace zmq
{

    template <bool HAS_IN, bool HAS_OUT> class engine_base_t : public i_engine
    {
    protected:

        engine_base_t (mux_t *mux_, i_demux *demux_/*,
              bool load_balancing_ = false*/)
        {
            mux = mux_;
            demux = demux_;
        }

        ~engine_base_t ()
        {
        }

        i_pollable *cast_to_pollable ()
        {
            zmq_assert (false);
            return NULL;
        }

        const char *get_arguments ()
        {
            zmq_assert (false);
            return NULL;
        }

        void revive (pipe_t *pipe_)
        {
            //  Notify the reader of the pipe that there are messages
            //  available in the pipe.
            zmq_assert (HAS_OUT);
            mux->revive (pipe_);
        }

        void head (pipe_t *pipe_, int64_t position_)
        {
            //  Forward pipe head position to the appropriate pipe.
            zmq_assert (HAS_IN);
        }

        void send_to (pipe_t *pipe_)
        {
            //  Start sending messages to a pipe.
            zmq_assert (HAS_IN);
            demux->send_to (pipe_);
        }

        void receive_from (pipe_t *pipe_)
        {
            //  Start receiving messages from a pipe.
            zmq_assert (HAS_OUT);
            mux->receive_from (pipe_);
        }

        void terminate_pipe (pipe_t *pipe_)
        {
            //  Drop reference to the pipe.
            zmq_assert (HAS_IN);
            demux->release_pipe (pipe_);
        }

        void terminate_pipe_ack (pipe_t *pipe_)
        {
            //  Drop reference to the pipe.
            zmq_assert (HAS_OUT);
            mux->release_pipe (pipe_);
        }

        mux_t *mux;
        i_demux *demux;
    };

}

#endif
