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

#ifndef __ZMQ_PIPE_HPP_INCLUDED__
#define __ZMQ_PIPE_HPP_INCLUDED__

#include "i_context.hpp"
#include "i_engine.hpp"
#include "ypipe.hpp"
#include "msg.hpp"
#include "config.hpp"

namespace zmq
{

    class pipe_t
    {
    public:

        pipe_t (struct i_context *source_context_,
            struct i_engine *source_engine_,
            struct i_context *destination_context_,
            struct i_engine *destination_engine_);
        ~pipe_t ();

        inline void write (void *msg_)
        {
            pipe_element_t element;

            //  Pipe delimiter and large message are passed via reference.
            //  Very small messages (VSMs) are passed by value.
            if (!msg_ || msg_size (msg_) > max_vsm_size) {
                element.msg = msg_;
            }
            else {
                element.msg = (void*) pipe_element_t::vsm_tag;
                element.vsm_size = msg_size (msg_);
                memcpy (element.vsm_data, msg_data (msg_), msg_size (msg_));
            }
            pipe.write (element);
        }

        inline void flush ()
        {
            if (!pipe.flush ())
                send_revive ();
        }

        bool eop ()
        {
            return endofpipe;
        }

        void *read ();
        void revive ();
        void send_destroy_pipe ();

    private:

        void send_revive ();

        //  Contains 0MQ message ('msg' member). If the message is shorter than
        //  max_vsm_size it should be transferred by copy to avoid inter-thread
        //  contention in memory management infrastructure. In that case
        //  'msg' member is set to vsm_tag, vsm_size is set to message
        //  size and vsm_data contain the message body.
        struct pipe_element_t
        {
            enum {vsm_tag = 0x1};
            void *msg;
            uint16_t vsm_size;
            unsigned char vsm_data [max_vsm_size];
        };

        //  The message pipe itself
        typedef ypipe_t <pipe_element_t, false, message_pipe_granularity>
            underlying_pipe_t;
        underlying_pipe_t pipe;

        //  Identification of the engine sending the messages to the pipe
        i_context *source_context;
        i_engine *source_engine;

        //  Identification of the engine receiving the messages from the pipe
        i_context *destination_context;
        i_engine *destination_engine;

        bool alive; 
        bool endofpipe;
    }; 

}

#endif
