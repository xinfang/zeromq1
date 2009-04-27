require 'zmq'

class AssertionFailure < StandardError
end

def assert(bool, message = 'assertion failure')
    raise AssertionFailure.new(message) unless bool
end

	if ARGV.length != 4
		puts "usage: c_local_thr <hostname> <in-interface> "
           "<message-size> <roundtrip-count>"
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
    rb_zmq = Zmq.new(host);
	    
    #  Create the wiring.
	rb_zmq.create_queue("Q", ZMQ_SCOPE_GLOBAL, in_interface,
        ZMQ_NO_LIMIT, ZMQ_NO_LIMIT, ZMQ_NO_SWAP)
       
    data = rb_zmq.receive(ZMQ_TRUE)
	assert (data.msg.to_s.length == message_size.to_i)
        
    #  Get initial timestamp.
    start_time = Time.now
       
    #  The message loop.
    for i in 0...roundtrip_count.to_i do
    	data = rb_zmq.receive(ZMQ_TRUE)
    	assert (data.msg.to_s.length == message_size.to_i)
    end
    
    #  Get terminal timestamp.
    end_time = Time.now
    
    #  Compute and print out the throughput.
    message_throughput = roundtrip_count.to_i / (end_time.to_f - start_time.to_f);
    megabit_throughput = message_throughput.to_f * message_size.to_i * 8 /
        1000000;
    puts "Your average throughput is " + message_throughput.to_s + "[msg/s]"
    puts "Your average throughput is " + megabit_throughput.to_s + "[Mb/s]"    


