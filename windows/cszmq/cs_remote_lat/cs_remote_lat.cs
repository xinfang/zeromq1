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

    static unsafe int Main (string[] args)
    {
        if (args.Length != 5)
        {
            Console.Out.WriteLine ("usage: remote_lat <hostname> " +
                "<in-interface> <out-interface> " +
                "<message-size> <roundtrip-count>\n");
            return 1;
        }

        String host = args [0];
        String in_interface = args [1];
        String out_interface = args [2];
        uint msg_size = Convert.ToUInt32 (args [3]);
        int num_msg = Convert.ToInt32 (args [4]);

        //  Create 0MQ Dnzmq class.
        Dnzmq w = new Dnzmq (host);

        //  Set up 0MQ wiring.
        int eid = w.create_exchange ("EG", Dnzmq.ZMQ_SCOPE_GLOBAL,
            out_interface);
        int qid = w.create_queue ("QG", Dnzmq.ZMQ_SCOPE_GLOBAL,
            in_interface);
        
        //  Bounce the received messages.
        for (int i = 0; i < num_msg; i++)
        {
            byte [] msg = w.receive ();
            Debug.Assert (msg.Length == msg_size);                
            w.send (eid, msg);
        }

        System.Threading.Thread.Sleep (5000);

        return 0;
    }
}