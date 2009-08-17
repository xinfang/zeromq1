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

program pas_remote_lat;
{$APPTYPE CONSOLE}

uses
  SysUtils, ZeroMQ;
var
  Zmq: TZeroMQ;
  Buf: AnsiString;
  MsgType: Cardinal;
  Host: AnsiString;
  InInterface: AnsiString;
  OutInterface: AnsiString;
  MessageSize: Integer;
  RoundtripCount: Integer;
  Eid: Integer;
  Counter: Integer;

begin

    //  Parse command line arguments.
    if paramcount <> 5 then
    begin
        WriteLn ('usage: pas_remote_lat <Hostname> <In-Interface>' +
            '<Out-Interface> <Message-Size> <Roundtrip-Count>');
        Exit;
    end;

    Host := paramstr (1);
    InInterface := paramstr (2);
    OutInterface := paramstr (3);
    MessageSize := StrToInt (paramstr(4));
    RoundtripCount := StrToInt (paramstr(5));

    //  Print out the test parameters.
    WriteLn ('message size: ', MessageSize, ' [B]');
    WriteLn ('roundtrip count: ', RoundtripCount);

    //  Create 0MQ transport.
    Zmq := TZeroMQ.Create;

    Zmq.Open (Host);

    //  Create exchange and queue.
    Eid := Zmq.CreateExchange ('EG', zmqScopeGlobal, OutInterface,
      zmqStyleLoadBalancing,);
    Zmq.CreateQueue ('QG', zmqScopeGlobal, InInterface);


    //  The message loop.
    for Counter := 0 to RoundtripCount do
    begin
        Zmq.Receive (Buf, MsgType, zmqBlocking);
        Assert (Length (Buf) = MessageSize);
        Zmq.Send (Eid, Buf, zmqBlocking);
    end;

    Zmq.Close;
end.





