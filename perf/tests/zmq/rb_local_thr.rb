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

	if ARGV.length != 4
		puts "usage: rb_local_thr <hostname> <in-interface> <message-size>" + \
		" <roundtrip-count>"
        Process.exit
    end

	host = ARGV[0]
	in_interface = ARGV[1]
   	message_size = ARGV[2]
	roundtrip_count = ARGV[3]
	
	#  Print out the test parameters.
    puts "message size: " + message_size.to_s + " [B]"
	puts "message count: " + roundtrip_count.to_s
						
	#  Create 0MQ transport.
    rb_zmq = Zmq.new(host.to_s);
    	    
    #  Create the wiring.
	rb_zmq.create_queue("Q", ZMQ_SCOPE_GLOBAL, in_interface,
        ZMQ_NO_LIMIT, ZMQ_NO_LIMIT, ZMQ_NO_SWAP)
   
    data = rb_zmq.receive(ZMQ_TRUE)
	assert (data.msg.to_s.length == message_size.to_i)
	    
    #  Get initial timestamp.
    start_time = Time.now
       
    #  The message loop.
    for i in 0...roundtrip_count.to_i-1 do
    	data = rb_zmq.receive(ZMQ_TRUE)
    	assert (data.msg.to_s.length == message_size.to_i)
    end
 
    #  Get terminal timestamp.
    end_time = Time.now
    
    #  Compute and print out the throughput.
    if end_time.to_f - start_time.to_f != 0
    	message_throughput = roundtrip_count.to_i / 
    		(end_time.to_f - start_time.to_f);
    else
    	message_throughput = roundtrip_count.to_i
    end
    
    megabit_throughput = message_throughput.to_f * message_size.to_i * 8 /
       1000000;
    puts "Your average throughput is " + "%0.2f" % message_throughput.to_s + 
    	" [msg/s]"
    puts "Your average throughput is " + "%0.2f" % megabit_throughput.to_s + 
    	" [Mb/s]"    


