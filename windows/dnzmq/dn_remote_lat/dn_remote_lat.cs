using System;
using System.Collections.Generic;
using System.Text;
using zmq;
using System.Runtime.InteropServices;

namespace remote_lat
{
    class dn_remote_lat
    {

        public const int ZMQ_SCOPE_GLOBAL = 1;
        public const int ZMQ_SCOPE_LOCAL = 0;

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

            //  Variables needed for conversion to pointer to sbyte.
            String qg = "QG";
            String eg = "EG";
            sbyte[] b_host = new sbyte[20];
            sbyte[] sb_out_interface = new sbyte[20];
            sbyte[] sb_in_interface = new sbyte[20];
            sbyte[] sb_qg = new sbyte[20];
            sbyte[] sb_eg = new sbyte[20];
            char[] c_host = new char[20];
            char[] c_out_interface = new char[20];
            char[] c_in_interface = new char[20];
            char[] c_qg = new char[20];
            char[] c_eg = new char[20];

            //  Conversion to char array.
            c_host = host.ToCharArray (0, host.Length);
            c_out_interface = out_interface.ToCharArray (0, out_interface.Length);
            c_in_interface = in_interface.ToCharArray (0, in_interface.Length);
            c_qg = qg.ToCharArray (0, qg.Length);
            c_eg = eg.ToCharArray (0, eg.Length);

            //  Conversion to sbytes.
            for (int i = 0; i < c_host.Length; i++)
                b_host[i] = (sbyte) c_host[i];
            for (int i = 0; i < out_interface.Length; i++)
                sb_out_interface[i] = (sbyte) c_out_interface[i];
            for (int i = 0; i < in_interface.Length; i++)
                sb_in_interface[i] = (sbyte) c_in_interface[i];
            for (int i = 0; i < qg.Length; i++)
                sb_qg[i] = (sbyte) c_qg[i];
            for (int i = 0; i < eg.Length; i++)
                sb_eg[i] = (sbyte) c_eg[i];

            //  Create 0MQ transport.
            fixed (sbyte* pb_host = b_host)
                w.create (pb_host);

            //  Create 0MQ exchange.
            fixed (sbyte* pb_eg = sb_eg)
            fixed (sbyte* pb_out_interface = sb_out_interface)
                ex = w.create_exchange (pb_eg, ZMQ_SCOPE_GLOBAL, pb_out_interface);

            //  Create 0MQ queue.
            fixed (sbyte* pb_qg = sb_qg)
            fixed (sbyte* pb_in_interface = sb_in_interface)
                que = w.create_queue (pb_qg, ZMQ_SCOPE_GLOBAL, pb_in_interface);

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

