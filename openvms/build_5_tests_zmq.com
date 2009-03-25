$! BUILD_5_TESTS_ZMQ.COM
$! 2009-02-17
$! Modified:
$!    2009-03-18 ja - changed to relect new zmqRoot definition
$!+
$! Build the ZMQ test programs.
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"	! save default
$ set default zmqRoot:[perf.tests.zmq]         ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit local_lat.cpp
$ compit pgm_local_lat.cpp
$ compit local_thr.cpp
$ compit pgm_local_thr.cpp
$ compit remote_lat.cpp
$ compit pgm_remote_lat.cpp
$ compit remote_thr.cpp
$ compit pgm_remote_thr.cpp
$!
$! Note: 
$! the "foreign commands" for these programmes are
$! defined by setEnv.com
$!
$ linkit local_lat.obj, 	libzmq:libzmq.olb/lib
$ linkit pgm_local_lat.obj, 	libzmq:libzmq.olb/lib
$ linkit local_thr.obj, 	libzmq:libzmq.olb/lib
$ linkit pgm_local_thr.obj, 	libzmq:libzmq.olb/lib
$ linkit remote_lat.obj,	libzmq:libzmq.olb/lib
$ linkit pgm_remote_lat.obj,	libzmq:libzmq.olb/lib
$ linkit remote_thr.obj,	libzmq:libzmq.olb/lib
$ linkit pgm_remote_thr.obj,	libzmq:libzmq.olb/lib
$
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
