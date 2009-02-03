Module vb_remote_thr
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main()

        '  Parse the command line.
        Dim Arguments() As String = Split(Command$)
        If Arguments.Count <> 3 Then
            Console.Write("usage: vb_remote_lat <hostname> <message-size> ")
            Console.WriteLine("<message-count>")
            Return
        End If
        Dim Host As String = Arguments(0)
        Dim MessageSize As Integer = Arguments(1)
        Dim RoundtripCount As Integer = Arguments(2)

        '  Create 0MQ Dnzmq class.
        Dim Transport As New Dnzmq(Host)

        '  Set up 0MQ wiring.
        Dim ExchangeId As Integer = Transport.create_exchange("E", Dnzmq.SCOPE_LOCAL, "")
        Transport.bind("E", "Q")

        '  Create a message to send.
        Dim Message(MessageSize - 1) As Byte

        '  Start sending messages.
        For i As Integer = 0 To RoundtripCount + 1
            Transport.send(ExchangeId, Message)
        Next

        Sleep(5000)

    End Sub

End Module
