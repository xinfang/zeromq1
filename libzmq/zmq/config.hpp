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

#ifndef __ZMQ_CONFIG_HPP_INCLUDED__
#define __ZMQ_CONFIG_HPP_INCLUDED__

namespace zmq
{

    //  Compile-time settings for 0MQ

    enum 
    {
        //  Default port to use to connect to global locator (zmq_server).
        default_locator_port = 5682,

        //  Maximal batching size for incoming backend protocol messages.
        //  So, if there are 10 messages that fit into the batch size, all of
        //  them may be read by a single 'read' system call, thus avoiding
        //  unnecessary network stack traversals.
        bp_in_batch_size = 8192,

        //  Maximum transport data unit size for PGM (TPDU).
        pgm_max_tpdu = 1500,

        //  PGM engine buffer (receiver).
        pgm_in_batch_size = 1000,

        //  The OpenPGM transmit/receive window size can be set by count of 
        //  sequence numbers pgm_window_size or by maximum transmit / receive 
        //  rate and a time interval.
        //  When pgm_window_size is 0 window is defined by pgm_max_rte and
        //  pgm_secs.
        pgm_window_size = 0,

        //  PGM maximum transmit/receive rate B/s.
        //   10mb :    1250000
        //  100mb :   12500000
        //    1gb :  125000000
        pgm_max_rte = 12500000,

        //  PGM reliability time interval.
        pgm_secs = 10,
    
        //  Maximal batching size for outgoing backend protocol messages.
        //  So, if there are 10 messages that fit into the batch size, all of
        //  them may be sent by a single 'write' system call, thus avoiding
        //  unnecessary network stack traversals.
        bp_out_batch_size = 8192,

        //  Number of new messages in message pipe needed to trigger new memory
        //  allocation.
        message_pipe_granularity = 256,

        //  Number of new commands in command pipe needed to trigger new memory
        //  allocation.
        command_pipe_granularity = 16,

        //  Maximal size of "Very Small Message". VSMs are passed by value
        //  to avoid excessive memory allocation/deallocation.
        max_vsm_size = 30,

        //  Determines how often does api_thread poll for new messages when it
        //  still has unprocessed messages to handle. Thus, if it is set to 100,
        //  api_thread will process 100 messages before doing the poll. If there
        //  are no unprocessed messages available, poll is done immediately.
        //  Decreasing the value trades overall latency for more real-time
        //  behaviour (less latency peaks).
        api_thread_poll_rate = 100,

        //  Maximal delay to process command in API thread (in CPU tics).
        //  This setting is used only on x86 platform with GCC or MSVC compiler.
        api_thread_max_command_delay = 3000000,

        //  Maximum number of events the wait() method of I/O thread
        //  can process in one go.
        max_io_events = 256,

        //  High and low watermark for backend protocol engines.
        bp_hwm = 10000,
        bp_lwm = 5000,

        //  Due to unimplemented "explicit EOR" mechanism in Linux kernel
        //  implementation of SCTP we are not able to send SCTP messages
        //  larger than SCTP tx buffer. This setting allows to set the upper
        //  limit for SCTP message size.
        max_sctp_message_size = 4096,

        //  Maximal wait time when engine sets timer (milliseconds).
        max_timer_period = 100
    };

}

#endif
