Module vb_remote_lat
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
   
    Sub Main()

        '  Parse the command line.
        Dim Arguments() As String = Split(Command$)
        If Arguments.Count <> 5 Then
            Console.Write("usage: vb_remote_lat <hostname> <in-interface>")
            Console.WriteLine("<out-interface> <message-size> <roundtrip-count>")
            Return
        End If
        Dim Host As String = Arguments(0)
        Dim InInterface As String = Arguments(1)
        Dim OutInterface As String = Arguments(2)
        Dim MsgSize As Integer = Arguments(3)
        Dim RoundtripCount As Integer = Arguments(4)

        '  Create 0MQ Dnzmq class.
        Dim Transport As New Dnzmq(Host)

        '  Set up 0MQ wiring.
        Dim ExchangeId As Integer = Transport.create_exchange("EG", Dnzmq.SCOPE_GLOBAL, OutInterface)
        Transport.create_queue("QG", Dnzmq.SCOPE_GLOBAL, InInterface)

        '  Start sending messages.
        For i As Integer = 0 To RoundtripCount
            Dim Msg() As Byte = Transport.receive()
            Debug.Assert(Msg.Length = MsgSize)
            Transport.send(ExchangeId, msg)
        Next

        Sleep(5000)

    End Sub

End Module
