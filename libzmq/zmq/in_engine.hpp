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

#ifndef __ZMQ_IN_ENGINE_HPP_INCLUDED__
#define __ZMQ_IN_ENGINE_HPP_INCLUDED__

#include <zmq/i_engine.hpp>
#include <zmq/mux.hpp>

namespace zmq
{

    class in_engine_t :public i_engine, public i_consumer
    {
    public:

        static in_engine_t *create (mux_t *mux_);

        bool read (message_t *msg_);

        //  i_engine implementation.
        void start (i_thread *current_thread_, i_thread *engine_thread_);
        class i_demux *get_demux ();
        class i_mux *get_mux ();

        //  i_consumer implementation.
        void revive ();
        void receive_from ();

    protected:
        //  i_engine implementation.
        const char *get_arguments ();

    private:

        in_engine_t (mux_t *mux_);
        ~in_engine_t ();

        mux_t *mux;
    };
}
#endif
