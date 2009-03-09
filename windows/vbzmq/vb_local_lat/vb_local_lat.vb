Imports zmq

Module vb_local_lat
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main()

        '  Parse the command line.
        Dim Arguments() As String = Split(Command$)
        If Arguments.Count <> 3 Then
            Console.Write("usage: vb_local_lat <hostname> <message-size> ")
            Console.WriteLine("<roundtrip-count>")
            Return
        End If
        Dim Host As String = Arguments(0)
        Dim MsgSize As Integer = Arguments(1)
        Dim RoundtripCount As Integer = Arguments(2)

        '  Print out the test parameters.  
        Console.WriteLine("message size: " & MsgSize & " [B]")
        Console.WriteLine("roundtrip count: " & RoundtripCount)

        '  Create 0MQ Dnzmq class.
        Dim Transport As New Dnzmq(Host)

        '  Set up 0MQ wiring.
        Dim ExchangeId As Integer = Transport.create_exchange("EL", Dnzmq.SCOPE_LOCAL, "", Dnzmq.STYLE_LOAD_BALANCING)
        Transport.create_queue("QL", Dnzmq.SCOPE_LOCAL, "", -1, -1, 0)
        Transport.bind("EL", "QG", "", "")
        Transport.bind("EG", "QL", "", "")

        '  Get initial timestamp. 
        Dim StartTime As DateTime = DateTime.Now()

        '  Start sending messages.
        Dim Msg(MsgSize - 1) As Byte
        Dim size As Int32

        For i As Integer = 0 To RoundtripCount
            Transport.send(ExchangeId, Msg)
            Transport.receive(Msg, size)
            Debug.Assert(size = MsgSize)
        Next

        '  Get final timestamp.  
        Dim EndTime As DateTime = DateTime.Now()
        Dim ElapsedTime As Int64 = (EndTime - StartTime).TotalMilliseconds

        '  Compute and print out the latency.
        Dim Latency As Double = ElapsedTime * 1000 / RoundtripCount / 2
        Console.WriteLine("Your average latency is {0} [us]", Latency.ToString("f2"))

        Sleep(5000)

    End Sub

End Module
