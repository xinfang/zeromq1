Imports Zmq

Module vb_remote_thr
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main()

        '  Parse the command line.
        Dim arguments() As String = Split(Command$)
        If arguments.Count <> 3 Then
            Console.Write("usage: vb_remote_lat <hostname> <message-size> ")
            Console.WriteLine("<message-count>")
            Return
        End If
        Dim host As String = arguments(0)
        Dim messageSize As Integer = arguments(1)
        Dim roundtripCount As Integer = arguments(2)

        '  Create 0MQ Zmq class.
        Dim transport As New Zmq(host)

        '  Set up 0MQ wiring.
        Dim exchangeId As Integer = transport.createExchange("E", Zmq.SCOPE_LOCAL, "", Zmq.STYLE_LOAD_BALANCING)
        transport.bind("E", "Q", "", "")

        '  Create a message to send.
        Dim message(messageSize - 1) As Byte

        '  Start sending messages.
        For i As Integer = 0 To roundtripCount + 1
            transport.send(exchangeId, message, True)
        Next

        Sleep(5000)

    End Sub

End Module
