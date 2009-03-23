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
using Zmq;

class CsRemoteThr
{
    static unsafe int Main (string [] args)
    {
        if (args.Length != 3)
        {
            Console.Out.WriteLine ("usage: cs_remote_thr <hostname> " +
            "<message-size> <message-count>\n");
            return 1;
        }

        String host = args[0];
        uint messageSize = Convert.ToUInt32 (args[1]);
        int msgCount = Convert.ToInt32 (args[2]);

        //  Create 0MQ Dnzmq class
        Dnzmq w = new Dnzmq (host);
        
        //  Set up 0MQ wiring.
        int exchange = w.CreateExchange ("E", Dnzmq.SCOPE_LOCAL, "", 
            Dnzmq.STYLE_LOAD_BALANCING);
        w.Bind ("E", "Q", null, null);

        //  Create a message to send.
        byte[] message = new byte[messageSize];

        //  Start sending messages.
        for (int i = 0; i < msgCount; i++)
            w.Send (exchange, message, true);
        
        System.Threading.Thread.Sleep (100000);

        w.Destroy ();
        return 0;
    }
}
