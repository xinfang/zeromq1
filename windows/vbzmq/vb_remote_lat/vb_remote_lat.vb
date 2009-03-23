Imports Zmq

Module vb_remote_lat
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main()

        '  Parse the command line.
        Dim arguments() As String = Split(Command$)
        If arguments.Count <> 5 Then
            Console.Write("usage: vb_remote_lat <hostname> <in-interface> ")
            Console.WriteLine("<out-interface> <message-size> <roundtrip-count>")
            Return
        End If
        Dim host As String = arguments(0)
        Dim inInterface As String = arguments(1)
        Dim outInterface As String = arguments(2)
        Dim messageSize As Integer = arguments(3)
        Dim roundtripCount As Integer = arguments(4)

        '  Create 0MQ Dnzmq class.
        Dim transport As New Zmq(host)

        '  Set up 0MQ wiring.
        Dim exchangeId As Integer = transport.createExchange("EG", Zmq.SCOPE_GLOBAL, outInterface, Zmq.STYLE_LOAD_BALANCING)
        transport.createQueue("QG", Zmq.SCOPE_GLOBAL, inInterface, Zmq.NO_LIMIT, Zmq.NO_LIMIT, Zmq.NO_SWAP)

        Dim size As Int32

        '  Start sending messages.
        For i As Integer = 0 To roundtripCount
            Dim message() As Byte
            message = Nothing
            transport.receive(message, size, True)
            Debug.Assert(message.Length = messageSize)
            transport.send(ExchangeId, message, True)
        Next

        Sleep(5000)

    End Sub

End Module
