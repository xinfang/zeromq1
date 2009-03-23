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
using System.Diagnostics;
using System.Runtime.InteropServices;

class cs_remote_lat
{

    static unsafe int Main (string [] args)
    {
        if (args.Length != 5)
        {
            Console.Out.WriteLine ("usage: cs_remote_lat <hostname> " +
                "<exchange_interface> <queue_interface> " +
                "<message-size> <roundtrip-count>\n");
            return 1;
        }

        String host = args [0];
        String exchangeInterface = args[1];
        String queueInterface = args [2];        
        uint messageSize = Convert.ToUInt32 (args [3]);
        int roundtripCount = Convert.ToInt32 (args [4]);

        //  Create 0MQ Dnzmq class.
        Zmq w = new Zmq (host);

        //  Set up 0MQ wiring.
        int exchange = w.CreateExchange ("EG", Zmq.SCOPE_GLOBAL,
            exchangeInterface, Zmq.STYLE_LOAD_BALANCING);
        int queue = w.CreateQueue ("QG", Zmq.SCOPE_GLOBAL,
            queueInterface, Zmq.NO_LIMIT, Zmq.NO_LIMIT, Zmq.NO_SWAP);

        byte[] message;
        int type;

        //  Bounce the received messages.
        for (int i = 0; i < roundtripCount; i++)
        {
            w.Receive (out message, out type, true);
            Debug.Assert (message.Length == messageSize);
            w.Send (exchange, message, true);
        }
       
        System.Threading.Thread.Sleep (5000);
        w.Destroy ();

        return 0;
    }
}
