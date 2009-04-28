require 'librbzmq'

class AssertionFailure < StandardError
end

def assert(bool, message = 'assertion failure')
    raise AssertionFailure.new(message) unless bool
end

	if ARGV.length != 3
		puts "usage: rb_remote_thr <hostname> <message-size> "
            "<roundtrip-count>"
		Process.exit
    end
        
	host = ARGV[0]
	message_size = ARGV[1]
	roundtrip_count = ARGV[2]
				
	#  Create 0MQ transport.
    rb_zmq = Zmq.new(host);
	    
    #  Create the wiring.
    eid = rb_zmq.create_exchange("E", ZMQ_SCOPE_LOCAL, "",
        ZMQ_STYLE_LOAD_BALANCING)
	rb_zmq.bind("E", "Q", "", "")
	    
    #  Create message data to send.
	out_buf = Array.new(message_size.to_i, "1")

	#  The message loop.
    for i in 0...roundtrip_count.to_i + 1 do
    	rb_zmq.send(eid, out_buf.to_s, message_size.to_i, ZMQ_TRUE)
   	end
   	
    #  Wait till all messages are sent.
    sleep 5
