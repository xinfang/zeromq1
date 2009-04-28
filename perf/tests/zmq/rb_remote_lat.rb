require 'librbzmq'

class AssertionFailure < StandardError
end

def assert(bool, message = 'assertion failure')
    raise AssertionFailure.new(message) unless bool
end


	if ARGV.length != 5
		puts "usage: c_remote_lat <hostname> <in-interface> <out-interface> "
           "<message-size> <roundtrip-count>"
        Process.exit
    end

	host = ARGV[0]
	in_interface = ARGV[1]
   	out_interface = ARGV[2]
	message_size = ARGV[3]
	roundtrip_count = ARGV[4]
						
	#  Create 0MQ transport.
    rb_zmq = Zmq.new(host);
	    
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
	

