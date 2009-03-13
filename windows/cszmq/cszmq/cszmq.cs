using System;
using System.Runtime.InteropServices;

namespace zmq
{

    public class Dnzmq : IDisposable
    {
        private bool isDisposed = false;
        private IntPtr zmq_;

        public const int SCOPE_LOCAL = 0;
        public const int SCOPE_GLOBAL = 1;

        public const int MESSAGE_DATA = 1;
        public const int MESSAGE_GAP = 2;

        public const int STYLE_DATA_DISTRIBUTION = 1;
        public const int STYLE_LOAD_BALANCING = 2;

        public Dnzmq ()
        {
            zmq_ = IntPtr.Zero;
        }

        public Dnzmq (string host)
        {
            Open (host);
        }

        ~Dnzmq ()
        {
            Dispose (false);
        }

        public void Open (string host)
        {
            zmq_ = czmq_create (host);
        }

        public bool IsOpen { get { return zmq_ == IntPtr.Zero; } }

        public void Mask (int message_mask)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            czmq_mask (zmq_, Convert.ToUInt32 (message_mask));
        }
        public int create_exchange (string exchange, int scope, string nic, int style)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            return czmq_create_exchange (zmq_, exchange, scope, nic, style);
        }

        public int create_queue (string queue, int scope, string nic, Int64 hwm, Int64 lwm, Int64 swapSize)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            return czmq_create_queue (zmq_, queue, scope, nic, hwm, lwm, swapSize);
        }

        public void bind (string exchange, string queue, string exchangeArgs, string queueArgs)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            czmq_bind (zmq_, exchange, queue, exchangeArgs, queueArgs);
        }

        class Pinner
        {
            public Pinner (GCHandle a_handle)
            {
                handle = a_handle;
            }
            public GCHandle handle;
            public void Unpin (IntPtr ptr)
            {
                handle.Free ();
            }
        };

        public void send (int eid, byte[] data)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");

            //  TODO: Commented code pins the memory down instead of copying
            //  the content. However, the performance result are undecisive.
            //  Check this out in the future.
            //
            //if (data.Length < 131072)
            //{
            IntPtr ptr = Marshal.AllocHGlobal (data.Length);
            Marshal.Copy (data, 0, ptr, data.Length);
            try
            {
                czmq_send (zmq_, eid, ptr,
                    Convert.ToUInt32 (data.Length), freeHGlobal);
            }
            catch
            {
                freeHGlobal (ptr);
            }
            //}
            //else
            //{
            //    Pinner pin = new Pinner (
            //        GCHandle.Alloc (data, GCHandleType.Pinned));
            //    try
            //    {
            //        czmq_send (zmq_, eid, pin.handle.AddrOfPinnedObject (),
            //            Convert.ToUInt32 (data.Length), pin.Unpin);
            //    }
            //    catch
            //    {
            //        pin.handle.Free ();
            //    }
            //}
        }

        public int receive (out byte[] data, out int type)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            IntPtr ptr;
            UInt32 dataSize;

            FreeMsgData freeFunc;
            UInt32 _type;
            int qid = czmq_receive (zmq_, out ptr, out dataSize, out freeFunc,
                out _type);
            type = (int) _type;

            if (ptr == IntPtr.Zero)
            {
                data = null;
                return qid;
            }

            data = new byte[dataSize];
            Marshal.Copy (ptr, data, 0, data.Length);
            if (freeFunc != null)
                freeFunc (ptr);
            return qid;
        }

        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        private delegate void FreeMsgData (IntPtr ptr);
        private static FreeMsgData freeHGlobal = Marshal.FreeHGlobal;

        public void Close ()
        {
            if (zmq_ != IntPtr.Zero)
            {
                czmq_destroy (zmq_);
                zmq_ = IntPtr.Zero;
            }
        }

        #region IDisposable Members

        public void Dispose ()
        {
            Dispose (true);
            GC.SuppressFinalize (this);
        }

        protected virtual void Dispose (bool disposing)
        {
            if (!isDisposed)
            {
                if (disposing)
                {
                    // dispose managed resources
                }
                Close ();
                isDisposed = true;
            }
        }

        #endregion

        #region C API

        [DllImport ("libczmq", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr czmq_create (string host);

        [DllImport ("libczmq", CallingConvention = CallingConvention.Cdecl)]
        static extern void czmq_destroy (IntPtr zmq);

        [DllImport ("libczmq", CallingConvention = CallingConvention.Cdecl)]
        static extern void czmq_mask (IntPtr zmq, UInt32 message_mask);

        [DllImport ("libczmq", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern int czmq_create_exchange (IntPtr zmq, string exchange,
            int scope, string nic, int style);

        [DllImport ("libczmq", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern int czmq_create_queue (IntPtr zmq, string queue, int scope,
            string nic, Int64 hwm, Int64 lwm, Int64 swapSize);

        [DllImport ("libczmq", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern void czmq_bind (IntPtr zmq, string exchange, string queue,
            string exchangeArgs, string queueArgs);

        [DllImport ("libczmq", CallingConvention = CallingConvention.Cdecl)]
        static extern void czmq_send (IntPtr zmq, int eid, IntPtr data_,
            UInt32 size, FreeMsgData ffn);

        [DllImport ("libczmq", CallingConvention = CallingConvention.Cdecl)]
        static extern int czmq_receive (IntPtr zmq, [Out] out IntPtr data,
             [Out] out UInt32 size, [Out] out FreeMsgData ffn,
             [Out] out UInt32 type);

        #endregion
    }
}