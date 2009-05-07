$! BUILD_2_LIBZMQ.COM
$! 2009-02-18
$! Modified
$!  2009-03-14 ja  - change to reflect new zmqRoot
$!  2009-04-13 BRC - support for Alpha (ensure all objects get into library)
$!
$!+
$! Build the libzmq modules and place them in an object library
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ cpu = "''f$getsyi("ARCH_NAME")'"
$ def = "''f$environment("DEFAULT")'"	! save default directory
$ set default  zmqRoot:[libzmq]		! go to the libzmq directory
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit tcp_socket.cpp
$ compit ysocketpair.cpp
$ compit ypollset.cpp
$ compit ysemaphore.cpp
$ compit dispatcher.cpp
$ compit err.cpp
$ compit bp_decoder.cpp
$ compit bp_encoder.cpp
$ compit api_thread.cpp
$ compit poll.cpp
$ compit epoll.cpp
$ compit devpoll.cpp
$ compit kqueue.cpp
$ compit bp_tcp_engine.cpp
$ compit mux.cpp
$ compit data_distributor.cpp
$ compit load_balancer.cpp
$ compit pipe.cpp
$ compit bp_tcp_listener.cpp
$ compit locator.cpp
$ compit tcp_listener.cpp
$ compit ip.cpp
$ compit thread.cpp
$ compit select.cpp
$ compit out_engine.cpp
$ compit in_engine.cpp
$ compit engine_factory.cpp
$ compit sctp_listener.cpp
$ compit sctp_engine.cpp
$ compit pgm_socket.cpp
$ compit bp_pgm_sender.cpp
$ compit bp_pgm_receiver.cpp
$ compit amqp_client.cpp
$ compit amqp_encoder.cpp
$ compit amqp_decoder.cpp
$ compit amqp_marshaller.cpp
$ compit amqp_unmarshaller.cpp
$ compit xmlParser.cpp
$ compit swap.cpp
$ compit io_thread.cpp
$!
$ lib/create libzmq.olb
$ lib/repl/nolog libzmq.olb *.obj;
$ if "''cpu'" .eqs. "Alpha"
$ then
$    lib/repl/nolog libzmq.olb [.cxx_repository]*.obj;
$ endif
$!
$ purge/nolog [...]
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
