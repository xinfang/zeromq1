/*
    Copyright (c) 2007-2008 FastMQ Inc.

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

class Jzmq
{
     //  Specifies that the object will be visible only in this session.
     public static final int SCOPE_LOCAL = 0;

     //  Specifies that the object will be visible all over the network.
     public static final int SCOPE_GLOBAL = 1;

     //  Initalises Jzmq. Hostname is a name or an IP address of the box where
     //  zmq_server is running.
     public Jzmq (String hostname)
     {
         construct (hostname);
     }

     //  Create an exchange. If the scope is global, you have supply the name
     //  or IP address of the network interface to be used by the exchange.
     //  Optionally you can specify the port to use this way "eth0:5555".
     //  Exchange ID is returned.
     public native int createExchange (String exchange, int scope, String nic);

     //  Create a queue. If the scope is global, you have supply the name
     //  or IP address of the network interface to be used by the exchange.
     //  Optionally you can specify the port to use this way "eth0:5555".
     //  Queue ID is returned.
     public native int createQueue (String queue, int scope, String nic);

     //  Bind the exchange to the queue.
     public native void bind (String exchange, String name);

     //  Send a binary message to the specified exchange.
     public native void send (int exchangeId, byte [] data);

     //  Receive next message.
     public native byte [] receive ();

     //  Initialises 0MQ infrastructure.
     protected native void construct (String hostname);

     //  Deallocates resources associated with the object.
     protected native void finalize ();

     protected long getContext ()
     {
         return context;
     }

     private long context;

     static
     {
         System.loadLibrary ("libjzmq");
     }
}
