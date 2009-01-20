Module vb_remote_thr
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

    Sub Main ()
        Dim args() As String
        args = Split (Command$)
        If args.Count <> 3 Then
            Console.Write ("usage: remote_lat <hostname> <message-size>")
            Console.WriteLine ("<roundtrip-count>")
            Return
        End If

        Dim host As String
        Dim msg_size As UInt32
        Dim roundtrip_count As Int32

        host = args.ElementAt (0)
        msg_size = Val (args.ElementAt (1))
        roundtrip_count = Val (args.ElementAt (2))

        '  Ids of exchange and queue.
        Dim eid As Integer

        '  Create 0MQ Dnzmq class.
        Dim w As New zmq.Dnzmq (host)

        '  Create 0MQ exchange.
        eid = w.create_exchange ("E", zmq.Dnzmq.ZMQ_SCOPE_LOCAL, host)

        '  Bind.
        w.bind ("E", "Q")

        '  Declare memory needed for messages.
        Dim msg (msg_size) As Byte

        '  Start sending messages.
        For i As Integer = 0 To roundtrip_count + 1
            w.send (eid, msg, msg_size)
        Next

        Sleep (5000)

    End Sub

End Module
