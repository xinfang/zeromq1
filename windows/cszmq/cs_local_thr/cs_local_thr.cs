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
                "<queue-interface> <message-size> <message-count>\n");
            return 1;
        }

        String host = args [0];
        String queueInterface = args [1];
        uint messageSize = Convert.ToUInt32 (args [2]);
        int msgCount = Convert.ToInt32 (args [3]);

        //  Print out the test parameters.
        Console.Out.WriteLine ("message size: " + messageSize + " [B]");
        Console.Out.WriteLine ("message count: " + msgCount);

        //  Create the Dnzmq class.
        Zmq w = new Zmq (host);

        //  Set up 0MQ wiring.
        int queue = w.CreateQueue ("Q", Zmq.SCOPE_GLOBAL, queueInterface,
            Zmq.NO_LIMIT, Zmq.NO_LIMIT, Zmq.NO_SWAP);

        byte[] message;
        int type;

        //  Receive the first message.
        w.Receive (out message, out type, true);
        Debug.Assert (message.Length == messageSize);

        //  Start measuring the time.
        System.Diagnostics.Stopwatch watch;
        watch = new Stopwatch ();
        watch.Start ();

        //  Start receiving messages
        for (int i = 0; i < msgCount - 1; i++)
        {
            w.Receive (out message, out type, true);
            Debug.Assert (message.Length == messageSize);
        }

        //  Stop measuring the time.
        watch.Stop ();
        Int64 elapsedTime = watch.ElapsedTicks;
        double time = elapsedTime / Stopwatch.Frequency;
        
        //  Compute and print out the throughput.
        Int64 messageThroughput;
        Int64 megabitThroughput;

        //message_throughput = (Int64) (msg_count / time);
        messageThroughput = (Int64) (msgCount *Stopwatch.Frequency / elapsedTime);
        megabitThroughput = messageThroughput * messageSize * 8 /
            1000000;
       
        Console.Out.WriteLine ("Your average throughput is {0} [msg/s]",
            messageThroughput.ToString ());
        Console.Out.WriteLine ("Your average throughput is {0} [Mb/s]",
            megabitThroughput.ToString ());

        System.Threading.Thread.Sleep (5000);
        w.Destroy ();

        return 0;
    }
}
