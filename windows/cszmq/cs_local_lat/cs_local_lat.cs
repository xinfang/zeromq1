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
using Zmq;

class cs_local_lat
{
    
    static unsafe int Main (string [] args)
    {
        if (args.Length != 3)
        {
            Console.Out.WriteLine ("usage: cs_local_lat <hostname> " +
                "<message-size> <roundtrip-count>\n");
            return 1;
        }
        
        String host = args [0];
        uint messageSize = Convert.ToUInt32 (args [1]);
        int roundtripCount = Convert.ToInt32 (args [2]);

        //  Print out the test parameters.
        Console.Out.WriteLine ("message size: " + messageSize + " [B]");
        Console.Out.WriteLine ("roundtrip count: " + roundtripCount);
        
        //  Create 0MQ Dnzmq class.
        Dnzmq w = new Dnzmq (host);

        //  Set up 0MQ wiring.
        int exchange = w.CreateExchange ("EL", Dnzmq.SCOPE_LOCAL, "", 
            Dnzmq.STYLE_LOAD_BALANCING);
        int queue = w.CreateQueue ("QL", Dnzmq.SCOPE_LOCAL, "",
            Dnzmq.NO_LIMIT, Dnzmq.NO_LIMIT, Dnzmq.NO_SWAP);
        w.Bind ("EL", "QG", null, null);
        w.Bind ("EG", "QL", null, null);

        //  Create a message to send.
        byte[] outMessage = new byte[messageSize];
        
        //  Start measuring the time.
        System.Diagnostics.Stopwatch watch;
        watch = new Stopwatch ();
        watch.Start ();


        byte[] inMessage;
        int type;
        //  Start sending messages.
        for (int i = 0; i < roundtripCount; i++)
        {
            w.Send (exchange, outMessage, true);
            w.Receive (out inMessage, out type, true);
            Debug.Assert (inMessage.Length == messageSize);
        }

        //  Stop measuring the time.
        watch.Stop ();
        Int64 elapsedTime = watch.ElapsedTicks;

        //  Compute and print out the latency.
        double latency = (double) (elapsedTime) / roundtripCount / 2 *
            1000000 / Stopwatch.Frequency;
        Console.Out.WriteLine ("Your average latency is {0} [us]",
            latency.ToString ("f2"));

        System.Threading.Thread.Sleep (5000);
        w.Destroy ();

        return 0;
    }
}