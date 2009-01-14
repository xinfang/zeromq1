using System;
using System.Collections.Generic;
using System.Text;
using zmq;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace gcTest
{

    class Local_lat
    {
        
        static unsafe int Main (string[] args)
        {
            if (args.Length != 3)
            {
                Console.Out.WriteLine ("usage: local_lat <hostname> <message-size> " +
                    "<roundtrip-count>\n");
                return 1;
            }

            String host = args[0];
            uint msg_size = Convert.ToUInt32 (args[1]);
            int roundtrip_count = Convert.ToInt32 (args[2]);

            /*  Print out the test parameters.  */
            Console.Out.WriteLine ("message size: " + msg_size + " [B]");
            Console.Out.WriteLine ("roundtrip count: " + roundtrip_count);

            //  Id of exchange
            int eid;

            //  Create 0MQ Dnzmq class.
            Dnzmq w = new Dnzmq ();

            //  Create 0MQ transport.
            w.create (host);

            //  Create 0MQ exchange.
            eid = w.create_exchange ("EL", Dnzmq.ZMQ_SCOPE_LOCAL, host);
            
            //  Create 0MQ queue.
            int que = w.create_queue ("QL", Dnzmq.ZMQ_SCOPE_LOCAL, host);
            
            //  Bind.
            w.bind ("EL", "QG");

            //  Bind.
            w.bind ("EG", "QL");

            //  Allocate memory needed for messages.
            byte[] msg = new byte[msg_size];
                        
            /*  Get initial timestamp.  */
            System.Diagnostics.Stopwatch watch;
            watch = new Stopwatch ();
            watch.Start ();

            //  Start sending messages.
            for (int i = 0; i < roundtrip_count; i++)
            {
                w.send (eid, msg, msg_size);
                w.receive ( ref msg, &msg_size);
            }

            /*  Get final timestamp.  */
            watch.Stop ();
            Int64 elapsed_time = watch.ElapsedTicks;

            /*  Compute and print out the latency.  */
            double latency = (double) (elapsed_time) / roundtrip_count / 2 * 100000 / Stopwatch.Frequency;
            Console.Out.WriteLine ("Your average latency is {0} [us]", latency.ToString ("f2"));
            
            System.Threading.Thread.Sleep (5000);
            return 0;
        }
    }
}

