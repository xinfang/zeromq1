using System;
using System.Runtime.InteropServices;

namespace Zmq
{

    public class Dnzmq : IDisposable
    {
        private bool isDisposed = false;
        private IntPtr zmq_;

        public const int SCOPE_LOCAL = 1;
        public const int SCOPE_PROCESS = 2;
        public const int SCOPE_GLOBAL = 3;

        public const int MESSAGE_DATA = 1;
        public const int MESSAGE_GAP = 2;

        public const int STYLE_DATA_DISTRIBUTION = 1;
        public const int STYLE_LOAD_BALANCING = 2;

        //  Defines watermark level.
        public const int NO_LIMIT = -1;

        public const int NO_SWAP = 0;

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

        public void Mask (int messageMask)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            czmq_mask (zmq_, Convert.ToUInt32 (messageMask));
        }
        public int CreateExchange (string name, int scope, string location, int style)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            return czmq_create_exchange (zmq_, name, scope, location, style);
        }

        public int CreateQueue (string name, int scope, string location, Int64 hwm, Int64 lwm, Int64 swap)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            return czmq_create_queue (zmq_, name, scope, location, hwm, lwm, swap);
        }

        public void Bind (string exchangeName, string queueName, string exchangeArgs, string queueArgs)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            czmq_bind (zmq_, exchangeName, queueName, exchangeArgs, queueArgs);
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

        public bool Send (int exchange, byte[] message, bool block)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");

            //  Return value determines if the message was sent.
            int sent = 0;

            //  TODO: Commented code pins the memory down instead of copying
            //  the content. However, the performance result are undecisive.
            //  Check this out in the future.
            //
            //if (data.Length < 131072)
            //{
            IntPtr ptr = Marshal.AllocHGlobal (message.Length);
            Marshal.Copy (message, 0, ptr, message.Length);
            try
            {
                sent = czmq_send (zmq_, exchange, ptr,
                    Convert.ToUInt32 (message.Length), freeHGlobal, Convert.ToInt32 (block));
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
            //        sent = czmq_send (zmq_, eid, pin.handle.AddrOfPinnedObject (),
            //            Convert.ToUInt32 (data.Length), pin.Unpin, Convert.ToInt32 (block));
            //    }
            //    catch
            //    {
            //        pin.handle.Free ();
            //    }
            //}

            return Convert.ToBoolean (sent);
            
        }

        public int Receive (out byte[] message, out int type, bool block)
        {
            if (zmq_ == IntPtr.Zero)
                throw new NullReferenceException ("queue must be initialized");
            IntPtr ptr;
            UInt32 messageSize;

            FreeMsgData freeFunc;
            UInt32 _type;
            int queue = czmq_receive (zmq_, out ptr, out messageSize, out freeFunc,
                out _type, Convert.ToInt32 (block));
            type = (int) _type;

            if (ptr == IntPtr.Zero)
            {
                message = null;
                return queue;
            }

            message = new byte[messageSize];
            Marshal.Copy (ptr, message, 0, message.Length);
            if (freeFunc != null)
                freeFunc (ptr);
            return queue;
        }

        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        private delegate void FreeMsgData (IntPtr ptr);
        private static FreeMsgData freeHGlobal = Marshal.FreeHGlobal;

        public void Destroy ()
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
                Destroy ();
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
        static extern void czmq_mask (IntPtr zmq, UInt32 notifications);

        [DllImport ("libczmq", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern int czmq_create_exchange (IntPtr zmq, string name,
            int scope, string location, int style);

        [DllImport ("libczmq", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern int czmq_create_queue (IntPtr zmq, string name, int scope,
            string location, Int64 hwm, Int64 lwm, Int64 swap);

        [DllImport ("libczmq", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern void czmq_bind (IntPtr zmq, string exchange_name, string queue_name,
            string exchange_options, string queue_options);

        [DllImport ("libczmq", CallingConvention = CallingConvention.Cdecl)]
        static extern int czmq_send (IntPtr zmq, int exchange, IntPtr data,
            UInt32 size, FreeMsgData ffn, int block);

        [DllImport ("libczmq", CallingConvention = CallingConvention.Cdecl)]
        static extern int czmq_receive (IntPtr zmq, [Out] out IntPtr data,
             [Out] out UInt32 size, [Out] out FreeMsgData ffn,
             [Out] out UInt32 type, int block);

        #endregion
    }
}