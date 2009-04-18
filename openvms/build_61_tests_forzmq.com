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
$ forit f_remote_lat.for
$ forit f_remote_thr.for
$ forit f_local_thr.for
$!
$ clinkit f_local_lat.obj, sys$input/opt
!
! This is the shareable image containing the wrappers for ZMQ
!
libvmszmq:vmszmq/share
!
$!
$ clinkit f_remote_lat.obj, sys$input/opt
!
! This is the shareable image containing the wrappers for ZMQ
!
libvmszmq:vmszmq/share
!
$!
$ clinkit f_remote_thr.obj, sys$input/opt
!
! This is the shareable image containing the wrappers for ZMQ
!
libvmszmq:vmszmq/share
!
$!
$ clinkit f_local_thr.obj, sys$input/opt
!
! This is the shareable image containing the wrappers for ZMQ
!
libvmszmq:vmszmq/share
!
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
