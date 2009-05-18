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

#ifndef __ZMQ_OUT_ENGINE_HPP_INCLUDED__
#define __ZMQ_OUT_ENGINE_HPP_INCLUDED__

#include <zmq/i_engine.hpp>
#include <zmq/i_demux.hpp>
#include <zmq/data_distributor.hpp>
#include <zmq/load_balancer.hpp>

namespace zmq
{

    class out_engine_t : public i_engine
    {
    public:

        static out_engine_t *create (i_demux *demux_);

        bool write (message_t &msg_);
        void flush ();

        //  i_engine implementation.
        void start (i_thread *current_thread_, i_thread *engine_thread_);
        void get_watermarks (int64_t *hwm_, int64_t *lwm_);
        int64_t get_swap_size ();
        class i_demux *get_demux ();
        class i_mux *get_mux ();
    
    protected:
        //  i_engine interface implementation.
        i_pollable *cast_to_pollable ();
        const char *get_arguments ();
        void revive ();
        void head ();
        void send_to (pipe_t *pipe_);
        void receive_from (pipe_t *pipe_);

    private:

        out_engine_t (i_demux *demux_);
        ~out_engine_t ();

        //  Engine demux
        i_demux *demux;

    };

}

#endif
