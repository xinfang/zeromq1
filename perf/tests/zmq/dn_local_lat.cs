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
        public const int ZMQ_SCOPE_GLOBAL = 1;
        public const int ZMQ_SCOPE_LOCAL = 0;

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

            //  Variables needed for conversion to pointers to sbytes.
            String ql = "QL";
            String el = "EL";
            String qg = "QG";
            String eg = "EG";
            sbyte[] sb_host = new sbyte[20];
            sbyte[] sb_ql = new sbyte[20];
            sbyte[] sb_el = new sbyte[20];
            sbyte[] sb_qg = new sbyte[20];
            sbyte[] sb_eg = new sbyte[20];
            char[] c_host = new char[20];
            char[] c_ql = new char[20];
            char[] c_el = new char[20];
            char[] c_qg = new char[20];
            char[] c_eg = new char[20];

            //  Conversion to char array.
            c_host = host.ToCharArray (0, host.Length);
            c_ql = ql.ToCharArray (0, ql.Length);
            c_host = host.ToCharArray (0, host.Length);
            c_el = el.ToCharArray (0, el.Length);
            c_qg = qg.ToCharArray (0, qg.Length);
            c_eg = eg.ToCharArray (0, eg.Length);

            //  Conversion to sbytes.
            for (int i = 0; i < c_host.Length; i++)
                sb_host[i] = (sbyte) c_host[i];
            for (int i = 0; i < ql.Length; i++)
                sb_ql[i] = (sbyte) c_ql[i];
            for (int i = 0; i < el.Length; i++)
                sb_el[i] = (sbyte) c_el[i];
            for (int i = 0; i < qg.Length; i++)
                sb_qg[i] = (sbyte) c_qg[i];
            for (int i = 0; i < eg.Length; i++)
                sb_eg[i] = (sbyte) c_eg[i];

            //  Create 0MQ transport.
            fixed (sbyte* pb_host = sb_host)
                w.create (pb_host);

            //  Create 0MQ exchange.
            fixed (sbyte* pb_el = sb_el)
            fixed (sbyte* pb_host = sb_host)
                eid = w.create_exchange (pb_el, ZMQ_SCOPE_LOCAL, pb_host);

            //  Create 0MQ queue.
            fixed (sbyte* pb_ql = sb_ql)
            fixed (sbyte* pb_host = sb_host)
                w.create_queue (pb_ql, ZMQ_SCOPE_LOCAL, pb_host);

            //  Bind.
            fixed (sbyte* pb_el = sb_el)
            fixed (sbyte* pb_qg = sb_qg)
                w.bind (pb_el, pb_qg);

            //  Bind.
            fixed (sbyte* pb_eg = sb_eg)
            fixed (sbyte* pb_ql = sb_ql)
                w.bind (pb_eg, pb_ql);

            //  Allocate memory needed for messages.
            void* v = (void*) Marshal.AllocCoTaskMem ((int) msg_size);


            sbyte[] sb_msg = new sbyte[msg_size];

            /*  Get initial timestamp.  */
            System.Diagnostics.Stopwatch watch;
            watch = new Stopwatch ();
            watch.Start ();

            //  Start sending messages.
            for (int i = 0; i < roundtrip_count; i++)
            {
                w.send (eid, v, msg_size);
                w.receive (&v, &msg_size);
            }

            /*  Get final timestamp.  */
            watch.Stop ();
            Int64 elapsed_time = watch.ElapsedTicks;

            /*  Compute and print out the latency.  */
            double latency = (double) (elapsed_time) / roundtrip_count / 2 * 100000 / Stopwatch.Frequency;
            Console.Out.WriteLine ("Your average latency is {0} [us]", latency.ToString ("f2"));

            //  Free allocated memory.
            Marshal.FreeCoTaskMem ((IntPtr) v);
            System.Threading.Thread.Sleep (5000);
            return 0;
        }
    }
}

