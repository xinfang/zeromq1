using System;
using System.Collections.Generic;
using System.Text;
using zmq;
using System.Runtime.InteropServices;

namespace remote_thr
{
    class dn_remote_thr
    {
        public const int ZMQ_SCOPE_GLOBAL = 1;
        public const int ZMQ_SCOPE_LOCAL = 0;

        static unsafe int Main (string[] args)
        {
            if (args.Length != 3)
            {
                Console.Out.WriteLine ("usage: remote_thr <hostname> <message-size> " +
                "<roundtrip-count>\n");
                return 1;
            }

            String host = args[0];
            uint msg_size = Convert.ToUInt32 (args[1]);
            int num_msg = Convert.ToInt32 (args[2]);

            //  Exchange id.
            int ex;

            //  Create 0MQ Dnzmq class
            Dnzmq w = new Dnzmq ();

            //  Variables needed for conversion of input parameters to pointers to sbytes.
            String eg = "E";
            String qg = "Q";
            sbyte[] sb_eg = new sbyte[20];
            sbyte[] sb_qg = new sbyte[20];
            char[] c_eg = new char[20];
            char[] c_qg = new char[20];
            sbyte[] b_host = new sbyte[20];
            char[] c_host = new char[20];

            //  Conversion to char array.
            c_eg = eg.ToCharArray (0, eg.Length);
            c_qg = qg.ToCharArray (0, qg.Length);
            c_host = host.ToCharArray (0, host.Length);

            //  Conversion to sbytes.
            for (int i = 0; i < c_host.Length; i++)
                b_host[i] = (sbyte) c_host[i];
            for (int i = 0; i < eg.Length; i++)
                sb_eg[i] = (sbyte) c_eg[i];
            for (int i = 0; i < qg.Length; i++)
                sb_qg[i] = (sbyte) c_qg[i];

            //  Create 0MQ transport.
            fixed (sbyte* pb_host = b_host)
                w.create (pb_host);

            //  Create 0MQ exchnge.           
            fixed (sbyte* pb_eg = sb_eg)
            fixed (sbyte* pb_host = b_host)
                ex = w.create_exchange (pb_eg, ZMQ_SCOPE_LOCAL, pb_host);

            //  Bind.
            fixed (sbyte* pb_eg = sb_eg)
            fixed (sbyte* pb_qg = sb_qg)
                w.bind (pb_eg, pb_qg);

            //  Allocate memory for messages.
            void* v = (void*) Marshal.AllocCoTaskMem ((int) msg_size);

            //  Start sending messages.
            for (int i = 0; i < num_msg + 1; i++)
                w.send (ex, v, msg_size);

            //  Free allocated memory.
            Marshal.FreeCoTaskMem ((IntPtr) v);
            System.Threading.Thread.Sleep (5000);
            return 0;
        }
    }
}

