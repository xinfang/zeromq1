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

#ifndef __ZMQ_CONFIG_HPP_INCLUDED__
#define __ZMQ_CONFIG_HPP_INCLUDED__

namespace zmq
{

    //  Compile-time settings for 0MQ
    enum
    {
        //  Size of input and output buffers for 0MQ backend protocol.
        bp_in_batch_size = 8192,
        bp_out_batch_size = 8192,

        //  Size of input and output buffers for AMQP protocol
        amqp_in_batch_size = 8192,
        amqp_out_batch_size = 8192,

        //  Number of messages to be enqueued before memory allocation happens.
        message_pipe_granularity = 256,

        //  Number of commands to be enqueued before memory allocation happens.
        command_pipe_granularity = 16,

        //  Setting maximal VSM size to 1 basically switches the functionality
        //  off. This parameter should be used with caution - it may solve
        //  your problem, if it is caused by inter-thread contention in the
        //  underlying memory management infrastructure, however, in most cases
        //  it will make 0MQ slower.
        max_vsm_size = 30
    };

}

#endif
