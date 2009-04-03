Imports Zmq

Module vb_local_lat
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main()

        '  Parse the command line.
        Dim arguments() As String = Split(Command$)
        If arguments.Count <> 3 Then
            Console.Write("usage: vb_local_lat <hostname> <message-size> ")
            Console.WriteLine("<roundtrip-count>")
            Return
        End If
        Dim host As String = arguments(0)
        Dim messageSize As Integer = arguments(1)
        Dim roundtripCount As Integer = arguments(2)

        '  Print out the test parameters.  
        Console.WriteLine("message size: " & messageSize & " [B]")
        Console.WriteLine("roundtrip count: " & roundtripCount)

        '  Create 0MQ Dnzmq class.
        Dim transport As New Zmq(host)

        '  Set up 0MQ wiring.
        Dim exchangeId As Integer = Transport.createExchange("EL", Zmq.SCOPE_LOCAL, "", Zmq.STYLE_LOAD_BALANCING)
        transport.createQueue("QL", Zmq.SCOPE_LOCAL, "", Zmq.NO_LIMIT, Zmq.NO_LIMIT, Zmq.NO_SWAP)
        transport.bind("EL", "QG", "", "")
        transport.bind("EG", "QL", "", "")

        '  Get initial timestamp. 
        Dim startTime As DateTime = DateTime.Now()

        '  Start sending messages.
        Dim message(messageSize - 1) As Byte
        Dim size As Int32

        For i As Integer = 0 To roundtripCount
            transport.send(exchangeId, message, True)
            transport.receive(message, size, True)
            Debug.Assert(size = messageSize)
        Next

        '  Get final timestamp.  
        Dim endTime As DateTime = DateTime.Now()
        Dim elapsedTime As Int64 = (endTime - startTime).TotalMilliseconds

        '  Compute and print out the latency.
        Dim latency As Double = elapsedTime * 1000 / roundtripCount / 2
        Console.WriteLine("Your average latency is {0} [us]", latency.ToString("f2"))

        Sleep(5000)

    End Sub

End Module
