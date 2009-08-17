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

program pas_local_lat;
{$APPTYPE CONSOLE}

uses
  SysUtils, Windows, ZeroMQ;
var
  Zmq: TZeroMQ;
  InBuf: AnsiString;
  OutBuf: AnsiString;
  MsgType: Cardinal;
  Host: AnsiString;
  MessageSize: Integer;
  RoundtripCount: Integer;
  Eid: Integer;
  Counter: Integer;
  Null : Variant;
  StartTime: Int64;
  EndTime: Int64;
  TicsDivM: Int64;
  Frequency: Int64;
  Latency: Real48;
begin

    //  Parse command line arguments.
    if paramcount <> 3 then
    begin
        WriteLn ('usage: pas_local_lat <Hostname> <Message-Size> ' +
            '<Roundtrip-Count>');
        Exit;
    end;

    Host := paramstr (1);
    MessageSize := StrToInt (paramstr(2));
    RoundtripCount := StrToInt (paramstr(3));

    //  Print out the test parameters.
    WriteLn ('message size: ', MessageSize, ' [B]');
    WriteLn ('roundtrip count: ', RoundtripCount);

    //  Create 0MQ transport.
    Zmq := TZeroMQ.Create;
    Zmq.Open (Host);

    //  Create queue and exchange.
    Eid := Zmq.CreateExchange ('EL', zmqScopeLocal, Null,
      zmqStyleLoadBalancing);
    Zmq.CreateQueue ('QL', zmqScopeLocal, Null);

    //  Bind.
    Zmq.Bind ('EL', 'QG');
    Zmq.Bind ('EG', 'QL');

    OutBuf := StringOfChar('1', MessageSize);

    //  Get initial timestamp.
    QueryPerformanceFrequency (Frequency);
    TicsDivM := trunc (Frequency / 1000000);
    QueryPerformanceCounter (StartTime);

    //  The message loop.
    for Counter := 0 to RoundtripCount do
    begin
        Zmq.Send (Eid, OutBuf, ZmqBlocking);
        Zmq.Receive (InBuf, MsgType, ZmqBlocking);
        Assert (Length (InBuf) = MessageSize);
    end;

    //  Get final timestamp.
    QueryPerformanceCounter (EndTime);

    StartTime := trunc (StartTime / TicsDivM);
    EndTime := trunc (EndTime / TicsDivM);

    //  Compute and print out the latency.
    Latency := (EndTime - StartTime) / RoundtripCount / 2;
    WriteLn ('Your average latency is ', Format('%f', [Latency]), ' [us]');

    Zmq.Close;
end.



