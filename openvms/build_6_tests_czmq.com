$! BUILD_6_TESTS_CZMQ.COM
$! 2009-02-23
$! Modified:
$!    2009-03-18 ja - changed to reflect new zmqRoot definition
$!+
$! Build the C ZMQ test programs.
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"	! save default
$ set default zmqRoot:[perf.tests.zmq]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ ccompit c_local_lat
$ ccompit c_remote_lat
$ ccompit c_local_thr
$ ccompit c_remote_thr
$!
$ linkit c_local_lat,  libczmq:libczmq.olb/lib,libzmq:libzmq.olb/lib
$ linkit c_remote_lat, libczmq:libczmq.olb/lib,libzmq:libzmq.olb/lib
$ linkit c_local_thr,  libczmq:libczmq.olb/lib,libzmq:libzmq.olb/lib
$ linkit c_remote_thr, libczmq:libczmq.olb/lib,libzmq:libzmq.olb/lib
$!
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
