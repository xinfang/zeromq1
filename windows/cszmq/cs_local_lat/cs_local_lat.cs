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
using zmq;

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
        uint msg_size = Convert.ToUInt32 (args [1]);
        int roundtrip_count = Convert.ToInt32 (args [2]);

        //  Print out the test parameters.
        Console.Out.WriteLine ("message size: " + msg_size + " [B]");
        Console.Out.WriteLine ("roundtrip count: " + roundtrip_count);
        
        //  Create 0MQ Dnzmq class.
        Dnzmq w = new Dnzmq (host);

        //  Set up 0MQ wiring.
        int eid = w.create_exchange ("EL", Dnzmq.SCOPE_LOCAL, "", Dnzmq.STYLE_LOAD_BALANCING);
        int qid = w.create_queue ("QL", Dnzmq.SCOPE_LOCAL, "", -1, -1, 0);
        w.bind ("EL", "QG", null, null);
        w.bind ("EG", "QL", null, null);

        //  Create a message to send.
        byte [] out_msg = new byte [msg_size];
        
        //  Start measuring the time.
        System.Diagnostics.Stopwatch watch;
        watch = new Stopwatch ();
        watch.Start ();


        byte[] in_msg;
        int type;
        //  Start sending messages.
        for (int i = 0; i < roundtrip_count; i++)
        {
            w.send (eid, out_msg, Dnzmq.TRUE);
            w.receive (out in_msg, out type, Dnzmq.TRUE);
            Debug.Assert (in_msg.Length == msg_size);
        }

        //  Stop measuring the time.
        watch.Stop ();
        Int64 elapsed_time = watch.ElapsedTicks;

        //  Compute and print out the latency.
        double latency = (double) (elapsed_time) / roundtrip_count / 2 *
            1000000 / Stopwatch.Frequency;
        Console.Out.WriteLine ("Your average latency is {0} [us]",
            latency.ToString ("f2"));

        System.Threading.Thread.Sleep (5000);
 
        return 0;
    }
}