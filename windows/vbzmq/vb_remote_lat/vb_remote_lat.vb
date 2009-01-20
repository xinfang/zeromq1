Module vb_remote_lat
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
   
    Sub Main ()
        Dim args () As String
        args = Split (Command$)
        If args.Count <> 5 Then
            Console.Write ("usage: remote_lat <hostname> <in-interface>")
            Console.WriteLine ("<out-interface> <message-size> <roundtrip-count>")
            Return
        End If

        Dim host As String
        Dim in_interface As String
        Dim out_interface As String
        Dim msg_size As UInt32
        Dim size As UInt32
        Dim roundtrip_count As Int32

        host = args.ElementAt (0)
        in_interface = args.ElementAt (1)
        out_interface = args.ElementAt (2)
        msg_size = Val (args.ElementAt (3))
        roundtrip_count = Val (args.ElementAt (4))

        '  Ids of exchange and queue.
        Dim eid As Integer

        '  Create 0MQ Dnzmq class.
        Dim w As New zmq.Dnzmq (host)

        '  Create 0MQ exchange.
        eid = w.create_exchange ("EG", zmq.Dnzmq.ZMQ_SCOPE_GLOBAL, out_interface)

        '  Create 0MQ queue.
        w.create_queue ("QG", zmq.Dnzmq.ZMQ_SCOPE_GLOBAL, in_interface)

        '  Declare memory needed for messages.
        Dim msg (msg_size) As Byte

        '  Start sending messages.
        For i As Integer = 0 To roundtrip_count
            size = w.receive (msg)
            Debug.Assert (size = msg_size)
            w.send (eid, msg, msg_size)
        Next

        Sleep (5000)

    End Sub

End Module
