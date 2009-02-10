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

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;

class cs_local_thr
{
    static unsafe int Main (string [] args)
    {
        if (args.Length != 4)
        {
            Console.Out.WriteLine ("usage: cs_local_thr <hostname> " +
                "<in-interface> <message-size> <message-count>\n");
            return 1;
        }

        String host = args [0];
        String in_interface = args [1];
        uint msg_size = Convert.ToUInt32 (args [2]);
        int msg_count = Convert.ToInt32 (args [3]);

        //  Print out the test parameters.
        Console.Out.WriteLine ("message size: " + msg_size + " [B]");
        Console.Out.WriteLine ("message count: " + msg_count);

        //  Create the Dnzmq class.
        Dnzmq w = new Dnzmq (host);

        //  Set up 0MQ wiring.
        w.create_queue ("Q", Dnzmq.SCOPE_GLOBAL, in_interface, -1, -1, 0);

        //  Receive the first message.
        byte [] msg = w.receive ();
        Debug.Assert (msg.Length == msg_size);

        //  Start measuring the time.
        System.Diagnostics.Stopwatch watch;
        watch = new Stopwatch ();
        watch.Start ();

        //  Start receiving messages
        for (int i = 0; i < msg_count - 1; i++)
        {
            msg = w.receive ();
            Debug.Assert (msg.Length == msg_size);
        }

        //  Stop measuring the time.
        watch.Stop ();
        Int64 elapsed_time = watch.ElapsedTicks;
        double time = (double) elapsed_time / Stopwatch.Frequency;
        
        //  Compute and print out the throughput.
        Int64 message_throughput;
        Int64 megabit_throughput;

        message_throughput = (Int64) (msg_count / time);
        megabit_throughput = message_throughput * msg_size * 8 /
            1000000;
       
        Console.Out.WriteLine ("Your average throughput is {0} [msg/s]",
            message_throughput.ToString ());
        Console.Out.WriteLine ("Your average throughput is {0} [Mb/s]",
            megabit_throughput.ToString ());

        System.Threading.Thread.Sleep (5000);

        return 0;
    }
}
