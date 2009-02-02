Module vb_local_thr
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main()

        '  Parse the command line.
        Dim Arguments() As String = Split(Command$)
        If Arguments.Count <> 4 Then
            Console.Write("usage: vb_local_thr <hostname> <interface>")
            Console.WriteLine("<message-size> <message-count>")
            Return
        End If
        Dim Host As String = Arguments(0)
        Dim InInterface As String = Arguments(1)
        Dim MsgSize As Integer = Arguments(2)
        Dim MsgCount As Integer = Arguments(3)

        '  Print out the test parameters.  
        Console.WriteLine("message size: " & MsgSize & " [B]")
        Console.WriteLine("message count: " & MsgCount)

        '  Create 0MQ Dnzmq class.
        Dim Transport As New Dnzmq(Host)

        '  Set up 0MQ wiring.
        Transport.create_queue("Q", Dnzmq.SCOPE_GLOBAL, InInterface)

        '  Receive the first message.
        Dim Msg() As Byte = Transport.receive()
        Debug.Assert(Msg.Length = MsgSize)

        '  Get initial timestamp. 
        Dim StartTime As DateTime = DateTime.Now()

        '  Start receiving messages.
        For i As Integer = 0 To MsgCount
            Msg = Transport.receive()
            Debug.Assert(Msg.Length = MsgSize)
        Next

        '  Get final timestamp.  
        Dim EndTime As DateTime = DateTime.Now()
        Dim ElapsedTime As Int64 = (EndTime - StartTime).TotalMilliseconds()

        '  Prevent division by 0.
        If (ElapsedTime = 0) Then
            ElapsedTime = 1
        End If

        '  Compute and print out the throughput.  
        Dim MsgThroughput As Double
        Dim MegabitThroughput As Double
        MsgThroughput = 1000 * MsgCount / (ElapsedTime)
        MegabitThroughput = MsgThroughput * MsgSize * 8 / 1000000
        Console.WriteLine("Your average throughput is {0} [msg/s]", MsgThroughput.ToString("f2"))
        Console.WriteLine("Your average throughput is {0} [Mb/s]", MegabitThroughput.ToString("f2"))

        Sleep(5000)

    End Sub

End Module
