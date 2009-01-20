Module vb_local_thr
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main ()
        Dim args () As String
        args = Split (Command$)
        If args.Count <> 4 Then
            Console.Write ("usage: local_thr <hostname> <interface>") 
            Console.WriteLine ("<message-size> <message-count>")
            Return
        End If

        Dim host As String
        Dim iface As String
        Dim msg_size As UInt32
        Dim size As UInt32
        Dim roundtrip_count As Int32

        host = args.ElementAt (0)
        iface = args.ElementAt (1)
        msg_size = Val (args.ElementAt (2))
        roundtrip_count = Val (args.ElementAt (3))

        '  Print out the test parameters.  
        Console.WriteLine ("message size: " & msg_size & " [B]")
        Console.WriteLine ("roundtrip count: " & roundtrip_count)

        '  Id of queue.
        Dim que As Integer

        '  Create 0MQ Dnzmq class.
        Dim w As New zmq.Dnzmq (host)

        '  Create 0MQ queue.
        que = w.create_queue ("Q", zmq.Dnzmq.ZMQ_SCOPE_GLOBAL, iface)

        '  Declare memory needed for messages.
        Dim msg (msg_size) As Byte

        '  Receive the first message.
        size = w.receive (msg)
        Debug.Assert (size = msg_size)

        '  Get initial timestamp. 
        Dim StartTime As DateTime = DateTime.Now ()

        '  Start receiving messages.
        For i As Integer = 0 To roundtrip_count
            size = w.receive (msg)
            Debug.Assert (size = msg_size)
        Next
        
        '  Get final timestamp.  
        Dim EndTime As DateTime = DateTime.Now ()
        Dim elapsed_time As Int64 = (EndTime - StartTime).TotalMilliseconds ()

        '  To prevent division by 0.
        If (elapsed_time = 0) Then
            elapsed_time = 1
        End If

        '  Compute and print out the throughput.  
        Dim message_throughput As Double
        Dim megabit_throughput As Double
        message_throughput = 1000 * roundtrip_count / (elapsed_time)
        megabit_throughput = message_throughput * msg_size * 8 / 1000000

        Console.WriteLine ("Your average throughput is {0} [msg/s]", message_throughput.ToString ("f2"))
        Console.WriteLine ("Your average throughput is {0} [Mb/s]", megabit_throughput.ToString ("f2"))

        Sleep (5000)



    End Sub

End Module
