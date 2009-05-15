#
#    Copyright (c) 2007-2009 FastMQ Inc.
#
#    This file is part of 0MQ.
#
#    0MQ is free software; you can redistribute it and/or modify it under
#    the terms of the Lesser GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    0MQ is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    Lesser GNU General Public License for more details.
#
#    You should have received a copy of the Lesser GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

require 'librbzmq'

class AssertionFailure < StandardError
end

def assert(bool, message = 'assertion failure')
    raise AssertionFailure.new(message) unless bool
end

	if ARGV.length != 3
		puts "usage: rb_remote_thr <hostname> <message-size> <roundtrip-count>"
		Process.exit
    end
        
	host = ARGV[0]
	message_size = ARGV[1]
	roundtrip_count = ARGV[2]
				
	#  Create 0MQ transport.
    rb_zmq = Zmq.new(host.to_s);
	    
    #  Create the wiring.
    eid = rb_zmq.create_exchange("E", ZMQ_SCOPE_LOCAL, "",
        ZMQ_STYLE_LOAD_BALANCING)
	rb_zmq.bind("E", "Q", "", "")
	    
    #  Create message data to send.
	out_buf = String.new();
	for i in 0...message_size.to_i do
		out_buf [i.to_i] = '1'
	end
	
	#  The message loop.
    for i in 0...roundtrip_count.to_i + 1 do
    	rb_zmq.send(eid, out_buf.to_s, message_size.to_i, ZMQ_TRUE)
   	end
   	
    #  Wait till all messages are sent.
    sleep 2
