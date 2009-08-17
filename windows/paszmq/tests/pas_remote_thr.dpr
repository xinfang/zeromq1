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

program pas_remote_thr;
{$APPTYPE CONSOLE}

uses
  SysUtils,
  Windows,
  ZeroMQ;

var
  Zmq: TZeroMQ;
  OutBuf: AnsiString;
  Host: AnsiString;
  MessageSize: Integer;
  MessageCount: Integer;
  Eid: Integer;
  Counter: Integer;
  Null : Variant;
begin

    //  Parse command line arguments.
    if paramcount <> 3 then
    begin
        WriteLn ('usage: pas_remote_thr <Hostname> <Message-Size> ' +
            '<Roundtrip-Count>');
        Exit;
    end;

    Host := paramstr (1);
    MessageSize := StrToInt (paramstr(2));
    MessageCount := StrToInt (paramstr(3));

    //  Print out the test parameters.
    WriteLn ('message size: ', MessageSize, ' [B]');
    WriteLn ('roundtrip count: ', MessageCount);

    //  Create 0MQ transport.
    Zmq := TZeroMQ.Create;
    Zmq.Open (Host);

    //  Create queue and exchange.
    Eid := Zmq.CreateExchange ('EL', zmqScopeLocal, Null,
      zmqStyleLoadBalancing);

    //  Bind.
    Zmq.Bind ('EL', 'QG');

    OutBuf := StringOfChar('1', MessageSize);

    //  The message loop.
    for Counter := 0 to MessageCount do
    begin
        Zmq.Send (Eid, OutBuf, ZmqBlocking);
    end;

    //  Wait till all messages are sent.
    Sleep (5000);

    Zmq.Close;
end.




