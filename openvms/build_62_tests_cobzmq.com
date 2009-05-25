$! BUILD_62_TESTS_COBZMQ.COM
$! 2009-05-20
$! Modified:
$!
$!+
$! Build the COBOL version of the ZMQ test programs.
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqRoot:[perf.tests.zmq]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ cobit cob_local_lat.cob
$! cobit cob_remote_lat.cob
$!
$ linkit cob_local_lat.obj, sys$input/opt
!
! This is the shareable image containing the wrappers for ZMQ
!
libvmszmq:vmszmq/share
!
$!
$! Change this when cob_remote_lat is written!
$!
$ goto No_cob_remote_yet
$ linkit cob_remote_lat.obj, sys$input/opt
!
! This is the shareable image containing the wrappers for ZMQ
!
libvmszmq:vmszmq/share
!
$ No_cob_remote_yet:
$!
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit

