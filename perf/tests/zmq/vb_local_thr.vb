Imports Zmq

Module vb_local_thr
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main()

        '  Parse the command line.
        Dim arguments() As String = Split(Command$)
        If arguments.Count <> 4 Then
            Console.Write("usage: vb_local_thr <hostname> <interface> ")
            Console.WriteLine("<message-size> <message-count>")
            Return
        End If
        Dim host As String = arguments(0)
        Dim inInterface As String = arguments(1)
        Dim messageSize As Integer = arguments(2)
        Dim msgCount As Integer = arguments(3)

        '  Print out the test parameters.  
        Console.WriteLine("message size: " & messageSize & " [B]")
        Console.WriteLine("message count: " & msgCount)

        '  Create 0MQ Dnzmq class.
        Dim transport As New Zmq(host)

        '  Set up 0MQ wiring.
        transport.createQueue("Q", Zmq.SCOPE_GLOBAL, inInterface, -1, -1, 0)

        '  Receive the first message.
        Dim message() As Byte
        Dim size As Int32
        message = Nothing
        
        transport.receive(message, size, True)
        Debug.Assert(size = messageSize)

        '  Get initial timestamp. 
        Dim startTime As DateTime = DateTime.Now()

        '  Start receiving messages.
        For i As Integer = 0 To msgCount
            transport.receive(message, size, True)
            Debug.Assert(message.Length = messageSize)
        Next

        '  Get final timestamp.  
        Dim endTime As DateTime = DateTime.Now()
        Dim elapsedTime As Int64 = (endTime - startTime).TotalMilliseconds()

        '  Prevent division by 0.
        If (elapsedTime = 0) Then
            elapsedTime = 1
        End If

        '  Compute and print out the throughput.  
        Dim msgThroughput As Double
        Dim megabitThroughput As Double
        msgThroughput = 1000 * msgCount / (elapsedTime)
        megabitThroughput = msgThroughput * messageSize * 8 / 1000000
        Console.WriteLine("Your average throughput is {0} [msg/s]", msgThroughput.ToString("f2"))
        Console.WriteLine("Your average throughput is {0} [Mb/s]", megabitThroughput.ToString("f2"))

        Sleep(5000)

    End Sub

End Module
