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

class j_local_lat
{
     public static void main (String [] args)
     {
         if (args.length != 3) {
             System.out.println ("usage: java j_local_lat <hostname> " +
                 "<message size> <roundtrip count>");
             return;
         }

         //  Parse the command line arguments.
         String host = args [0];
         int messageSize = Integer.parseInt (args [1]);
         int roundtripCount = Integer.parseInt (args [2]);

         //  Print out the test parameters.
         System.out.println ("message size: " + messageSize + " [B]");
         System.out.println ("roundtrip count: " + roundtripCount);

         //  Initialise 0MQ runtime.
         Zmq obj = new Zmq (host);
         
         //  Create the wiring.
         int eid = obj.createExchange ("EL", Zmq.SCOPE_LOCAL, null, 
		Zmq.STYLE_LOAD_BALANCING);
         obj.createQueue ("QL", Zmq.SCOPE_LOCAL, null, Zmq.NO_LIMIT,
             Zmq.NO_LIMIT, Zmq.NO_SWAP);
         obj.bind ("EL", "QG", null, null);
         obj.bind ("EG", "QL", null, null);
         
         //  Get the inital timestamp.
         long start = System.currentTimeMillis ();

         for (int i = 0; i != roundtripCount; i ++) {
             byte data [] = new byte [messageSize];
             obj.send (eid, data, true);
             byte [] data2 = obj.receive (true).message;
             assert (data == data2);
         }

         //  Get the final timestamp.
         long end = System.currentTimeMillis ();

         //  Compute and print out the latency.
         double latency = (double) ((end - start) * 1000) / roundtripCount / 2;
         System.out.println ("Your average latency is " + latency + " [us]");
     }
}
