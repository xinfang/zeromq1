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

unit ZeroMQ;

interface

uses
  ZMQ, SysUtils;
 
type
  TZeroMQScope = (
    zmqScopeLocal = 1,
    zmqScopeProcess = 2,
    zmqScopeGlobal = 3
    );
  TZeroMQStyle = (zmqStyleDataDistribution = 1, zmqStyleLoadBalancing = 2);
  TZeroExchange = Integer;
  TZeroQueueID = Cardinal;
  TZeroBlockingMode = (zmqBlocking = 1, zmqNoBlocking = 0);
  EZeroMQException = class(Exception)
  end;

  TZeroMQ = class(TInterfacedObject)
  protected
    FConnection: Pointer;
    FHost: AnsiString;
  public
    procedure Open(const Host: AnsiString);
    procedure Close;
    function CreateExchange(
      Exchange: ansistring;
      Scope: TZeroMQScope;
      Location: ansistring;
      Style: TZeroMQStyle): Integer;
    function CreateQueue(
      QueueName: ansistring;
      Scope: TZeroMQScope;
      Location: ansistring = ''): Integer;
    function CreateLocalExchange(Exchange: ansistring;
      Style: TZeroMQStyle): Integer;
    function CreateLocalQueue(QueueName: ansistring): Integer;
    procedure Bind(ExchangeName: ansistring;
                  QueueName: ansistring;
                  ExchangeOptions: ansistring = '';
                  QueueOptions: ansistring = '');
    function Send(Exchange: Integer;
                  Message: AnsiString;
                  Blocking: TZeroBlockingMode = zmqBlocking): Integer;
    function Receive(var Message: AnsiString;
                     var MessageType: Cardinal;
                     Blocking: TZeroBlockingMode = zmqBlocking): TZeroQueueID;
  end;


const
  ALL_INTERFACES = '*';

implementation

const
  ZMQ_NO_LIMIT = -1;
const
  ZMQ_NO_SWAP = 0;

{ TZeroMQ }

procedure TZeroMQ.Bind(ExchangeName: ansistring; QueueName: AnsiString;
                       ExchangeOptions: ansistring = ''; QueueOptions: ansistring = '');
begin
  zmq_bind(FConnection, PAnsiChar(ExchangeName), PAnsiChar(QueueName), PAnsiChar(ExchangeOptions), PAnsiChar(QueueOptions))
end;

procedure TZeroMQ.Close;
begin
  zmq_free(FConnection);
  FConnection := nil;
end;

function TZeroMQ.CreateExchange(Exchange: ansistring; Scope: TZeroMQScope;
  Location: ansistring; Style: TZeroMQStyle): Integer;
begin
  Result := zmq_create_exchange(
    FConnection, PAnsiChar(Exchange), Integer(Scope), PAnsiChar(Location), Integer(Style));
end;

function TZeroMQ.CreateLocalExchange(Exchange: ansistring; Style: TZeroMQStyle): Integer;
begin
  Result := zmq_create_exchange(
    FConnection, PAnsiChar(Exchange), Integer(zmqScopeLocal), PAnsiChar(''), Integer(Style));
end;


function TZeroMQ.CreateLocalQueue(QueueName: ansistring): Integer;
begin
  Result := CreateQueue(QueueName, zmqScopeLocal,'');
end;

function TZeroMQ.CreateQueue(QueueName: ansistring; Scope: TZeroMQScope;
  Location: ansistring = ''): Integer;
begin
  Result := zmq_create_queue(FConnection, PAnsiChar(QueueName), Integer(Scope), PAnsiChar(Location), ZMQ_NO_LIMIT, ZMQ_NO_LIMIT, ZMQ_NO_SWAP);
end;

procedure TZeroMQ.Open(const Host: AnsiString);
begin
  FConnection := nil;
  FConnection := zmq_create(PAnsiChar(FHost));
  if FConnection = nil then
    raise EZeroMQException.Create('Cannot connect to ZeroMQ server');
end;

function TZeroMQ.Receive(var Message: AnsiString; var MessageType: Cardinal; Blocking: TZeroBlockingMode = zmqBlocking): TZeroQueueID;
var
  Res: Int64;
  Data: PAnsiChar;
  MessageSize: Int64;
begin
  Message := '';
  Data := nil;
  MessageSize := 0;
  Res := zmq_receive(FConnection, Data, MessageSize, MessageType, Integer(Blocking));
  if (Res > 0) and (MessageSize > 0) then
    Message := Data;
  zmq_free(Data);
  Result := Res;
end;

function TZeroMQ.Send(Exchange: Integer; Message: AnsiString;
  Blocking: TZeroBlockingMode = zmqBlocking): Integer;
var
  DataSize: Int64;
begin
  DataSize := length(Message);
  Result := zmq_send(
    FConnection,
    Exchange,
    PAnsiChar(Message + #0),
    DataSize + SizeOf(ansichar),
    Integer(Blocking)
    );
end;

end.
