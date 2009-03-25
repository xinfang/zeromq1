$! BUILD_61_TESTS_FOR_ZMQ.COM
$! 2009-03-23
$! Modified:
$!    
$!+
$! Build the FORTRAN version of the ZMQ test programs.
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"	! save default
$ set default zmqRoot:[perf.tests.zmq]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ forit f_local_lat.for
$!
$ linkit f_local_lat.obj, sys$input/opt
!
! This is the shareable image containing the wrappers for ZMQ
!
libczmq:zmq_shr/share
!
$!
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
