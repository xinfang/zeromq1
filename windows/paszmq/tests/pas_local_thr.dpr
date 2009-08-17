{
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
}

program pas_local_thr;
{$APPTYPE CONSOLE}

uses
  SysUtils, Windows, ZeroMQ;

var
  Zmq: TZeroMQ;
  Buf: AnsiString;
  MsgType: Cardinal;
  Host: AnsiString;
  InInterface: AnsiString;
  MessageSize: Integer;
  MessageCount: Int64;
  Counter: Integer;
  MessageThroughput: UInt64;
  MegabitThroughput: Real48;
  StartTime: Int64;
  EndTime: Int64;
  TicsDivM: Int64;
  Frequency: Int64;
begin

    //  Parse command line arguments.
    if paramcount <> 4 then
    begin
        WriteLn ('usage: pas_local_thr <Hostname> <In-Interface>' +
            ' <Message-Size> <Message-Count>');
        Exit;
    end;

    Host := paramstr (1);
    InInterface := paramstr (2);
    MessageSize := StrToInt (paramstr(3));
    MessageCount := StrToInt (paramstr(4));

    //  Print out the test parameters.
    WriteLn ('message size: ', MessageSize, ' [B]');
    WriteLn ('roundtrip count: ', MessageCount);

    //  Create 0MQ transport.
    Zmq := TZeroMQ.Create;

    Zmq.Open (Host);

    //  Create exchange and queue.
    Zmq.CreateQueue ('QG', zmqScopeGlobal, InInterface);

    //  Receive first message.
    Zmq.Receive (Buf, MsgType, zmqBlocking);
    Assert (Length (Buf) = MessageSize);

    //  Get initial timestamp.
    QueryPerformanceFrequency (Frequency);
    TicsDivM := trunc (Frequency / 1000000);
    QueryPerformanceCounter (StartTime);

    //  The message loop.
    for Counter := 0 to MessageCount-1 do
    begin
        Zmq.Receive (Buf, MsgType, zmqBlocking);
        Assert (Length (Buf) = MessageSize);
    end;

    //  Get final timestamp.
    QueryPerformanceCounter (EndTime);

    StartTime := trunc (StartTime / TicsDivM);
    EndTime := trunc (EndTime / TicsDivM);

    //  Compute and print out the throughput.
    MessageThroughput := trunc (1000000 * MessageCount /
        (EndTime - StartTime));
    MegabitThroughput := MessageThroughput * MessageSize * 8 /
        1000000;

    WriteLn ('Your average throughput is ', MessageThroughput, ' [msg/s]');
    WriteLn ('Your average throughput is ', Format('%f', [MegabitThroughput]), ' [Mb/s]');
 
    Zmq.Close;
end.






