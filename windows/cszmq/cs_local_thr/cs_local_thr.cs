using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using zmq;


namespace local_thr
{
    class dn_local_thr
    {
        static unsafe int Main (string[] args)
        {
            if (args.Length != 4)
            {
                Console.Out.WriteLine ("usage: local_thr <hostname> <interface> " +
                    "<message-size> <roundtrip-count>\n");
                return 1;
            }

            String host = args[0];
            String iface = args[1];
            uint msg_size = Convert.ToUInt32 (args[2]);
            int roundtrip_count = Convert.ToInt32 (args[3]);

            /*  Print out the test parameters.  */
            Console.Out.WriteLine ("message size: " + msg_size + " [B]");
            Console.Out.WriteLine ("roundtrip count: " + roundtrip_count);

            //  Create the Dnzmq class.
            Dnzmq w = new Dnzmq ();

            //  Create 0MQ transport.
            w.create (host);

            //  Create 0MQ queue.
            w.create_queue ("Q", Dnzmq.ZMQ_SCOPE_GLOBAL, iface);

            //  Allocate memory for messages.
            byte[] msg = new byte[msg_size];

            //  Receive the first message.
            msg_size = w.receive (msg);

            /*  Get initial timestamp.  */
            System.Diagnostics.Stopwatch watch;
            watch = new Stopwatch ();
            watch.Start ();

            //  Start receiving messages
            for (int i = 0; i < roundtrip_count; i++)
                msg_size = w.receive (msg);

            /*  Get final timestamp.  */
            watch.Stop ();
            Int64 elapsed_time = watch.ElapsedTicks;
            double time = (double) elapsed_time / Stopwatch.Frequency;
            
            /*  Compute and print out the throughput.  */
            Int64 message_throughput;
            Int64 megabit_throughput;

            //  Elapsed_time should be divided by 10000, but then in 
            //  calculation of message throughput it should be multiplied
            //  by 100000, therefore  message_throuhput is calculated as 
            //  roundtrip_count /(time)) * 10.
            message_throughput = (Int64) (roundtrip_count /
                (time)) * 10;
            megabit_throughput = message_throughput * msg_size * 8 /
                1000000;
           
            Console.Out.WriteLine ("Your average throughput is {0} [msg/s]",
                message_throughput.ToString ());
            Console.Out.WriteLine ("Your average throughput is {0} [Mb/s]",
                megabit_throughput.ToString ());

            System.Threading.Thread.Sleep (5000);
            return 0;
        }

    }
}
