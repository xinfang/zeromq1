Module vb_local_lat
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
    
    Sub Main ()
        Dim args () As String
        args = Split (Command$)
        If args.Count <> 3 Then
            Console.Write ("usage: local_lat <hostname> <message-size>")
            Console.WriteLine ("<roundtrip-count>")
            Return
        End If

        Dim host As String
        Dim msg_size As UInt32
        Dim size As UInt32
        Dim roundtrip_count As Int32

        host = args.ElementAt (0)
        msg_size = Val (args.ElementAt (1))
        roundtrip_count = Val (args.ElementAt (2))

        '  Print out the test parameters.  
        Console.WriteLine ("message size: " & msg_size & " [B]")
        Console.WriteLine ("roundtrip count: " & roundtrip_count)

        '  Ids of exchange and queue.
        Dim eid As Integer
        Dim que As Integer

        '  Create 0MQ Dnzmq class.
        Dim w As New zmq.Dnzmq (host)

        '  Create 0MQ exchange.
        eid = w.create_exchange ("EL", zmq.Dnzmq.ZMQ_SCOPE_LOCAL, host)


        '  Create 0MQ queue.
        que = w.create_queue ("QL", zmq.Dnzmq.ZMQ_SCOPE_LOCAL, host)

        '  Bind.
        w.bind ("EL", "QG")

        '  Bind.
        w.bind ("EG", "QL")

        '  Declare memory needed for messages.
        Dim msg (msg_size) As Byte

        '  Get initial timestamp. 
        Dim StartTime As DateTime = DateTime.Now ()

        '  Start sending messages.
        For i As Integer = 0 To roundtrip_count
            w.send (eid, msg, msg_size)
            size = w.receive (msg)
            Debug.Assert (size = msg_size)
        Next


        '  Get final timestamp.  
        Dim EndTime As DateTime = DateTime.Now ()
        Dim elapsed_time As Int64 = (EndTime - StartTime).TotalMilliseconds

        '  Compute and print out the latency.
        Dim latency As Double
        latency = (elapsed_time) / roundtrip_count / 2 * 1000
        Console.WriteLine ("Your average latency is {0} [us]", latency.ToString ("f2"))

        Sleep (5000)

    End Sub

End Module
