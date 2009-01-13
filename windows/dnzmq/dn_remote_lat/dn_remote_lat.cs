using System;
using System.Collections.Generic;
using System.Text;
using zmq;
using System.Runtime.InteropServices;

namespace remote_lat
{
    class dn_remote_lat
    {

        static unsafe int Main (string[] args)
        {
            if (args.Length != 5)
            {
                Console.Out.WriteLine ("usage: remote_lat <hostname> <in-interface> <out-interface>" +
                    "<message-size> <roundtrip-count>\n");
                return 1;
            }

            String host = args[0];
            String in_interface = args[1];
            String out_interface = args[2];
            uint msg_size = Convert.ToUInt32 (args[3]);
            int num_msg = Convert.ToInt32 (args[4]);

            //  Exchange and queue ids
            int ex;
            int que;

            //  Create 0MQ Dnzmq class.
            Dnzmq w = new Dnzmq ();

            //  Create 0MQ transport.
            w.create (host);
            
            //  Create 0MQ exchange.
            ex = w.create_exchange ("EG", Dnzmq.ZMQ_SCOPE_GLOBAL, out_interface);
                        
            //  Create 0MQ queue.
            que = w.create_queue ("QG", Dnzmq.ZMQ_SCOPE_GLOBAL, in_interface);
            
            //  Allocate memory for messages.           
            void* v = (void*) Marshal.AllocCoTaskMem ((int) msg_size);

            //  Start sending and receiving.
            for (int i = 0; i < num_msg; i++)
            {
                w.receive (&v, &msg_size);
                w.send (ex, v, msg_size);
            }

            //  Free allocated memory.
            Marshal.FreeCoTaskMem ((IntPtr) v);
            System.Threading.Thread.Sleep (5000);
            return 0;
        }
    }
}

