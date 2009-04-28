require 'zmq'

class AssertionFailure < StandardError
end

def assert(bool, message = 'assertion failure')
    raise AssertionFailure.new(message) unless bool
end

	if ARGV.length != 3
		puts "usage: rb_local_lat <hostname> <message-size> "
            "<roundtrip-count>"
		Process.exit
    end
        
	host = ARGV[0]
	message_size = ARGV[1]
	roundtrip_count = ARGV[2]
				
	#  Print out the test parameters.
    puts "message size: #{message_size} [B]"
    puts "roundtrip count: #{roundtrip_count}"

	#  Create 0MQ transport.
    rb_zmq = Zmq.new(host);
	    
    #  Create the wiring.
    eid = rb_zmq.create_exchange("EL", ZMQ_SCOPE_LOCAL, "",
        ZMQ_STYLE_LOAD_BALANCING)
    rb_zmq.create_queue("QL", ZMQ_SCOPE_LOCAL, "",
        ZMQ_NO_LIMIT, ZMQ_NO_LIMIT, ZMQ_NO_SWAP)
	rb_zmq.bind("EL", "QG", "", "")
    rb_zmq.bind("EG", "QL", "", "")
	    
    #  Create message data to send.
	out_buf = Array.new(message_size.to_i, "1")
		

   	#  Get initial timestamp.
    start_time = Time.now

	#  The message loop.
    for i in 0...roundtrip_count.to_i do
    	rb_zmq.send(eid, out_buf.to_s, message_size.to_i, ZMQ_TRUE)
        in_buf = rb_zmq.receive(ZMQ_TRUE)
        assert (in_buf.msg.to_s.length == message_size.to_i)
   	end

    #  Get final timestamp.
  	end_time = Time.now

    #  Compute and print out the latency.
    latency = (end_time.to_f - start_time.to_f) *1000000 / 
    	roundtrip_count.to_i / 2
    puts "Your average latency is " +  "%0.2f" % latency  + "[us]"
