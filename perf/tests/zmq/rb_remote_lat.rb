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

	if ARGV.length != 5
		puts "usage: rb_remote_lat <hostname> <in-interface> <out-interface>" + \
		" <message-size> <roundtrip-count>"
        Process.exit
    end

	host = ARGV[0]
	in_interface = ARGV[1]
  	out_interface = ARGV[2]
	message_size = ARGV[3]
	roundtrip_count = ARGV[4]
						
	#  Create 0MQ transport.
    rb_zmq = Zmq.new(host.to_s);
	    
    #  Create the wiring.
	eid = rb_zmq.create_exchange("EG", ZMQ_SCOPE_GLOBAL, out_interface,
        ZMQ_STYLE_LOAD_BALANCING)
    rb_zmq.create_queue("QG", ZMQ_SCOPE_GLOBAL, in_interface,
        ZMQ_NO_LIMIT, ZMQ_NO_LIMIT, ZMQ_NO_SWAP)
        
    #  The message loop.
    for i in 0...roundtrip_count.to_i do
    	data = rb_zmq.receive(ZMQ_TRUE)
    	assert (data.msg.to_s.length == message_size.to_i)
        rb_zmq.send(eid, data.msg.to_s, data.msg.to_s.length, ZMQ_TRUE)	        
    end

sleep 2
	

