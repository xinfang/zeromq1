$! BUILD_7_TESTS_TCP.COM
$! 2009-02-17
$! Modified:
$!    2009-03-17 ja - changed to reflect new zmqRoot definition
$!+
$! Build the modules for testing
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES
$!
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqRoot:[perf.tests.tcp]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit local_lat.cpp
$ compit local_thr.cpp
$ compit remote_lat.cpp
$ compit remote_thr.cpp
$!
$! Now link the programs
$!
$ linkit local_lat.obj, libzmq:libzmq.olb/lib
$ linkit local_thr.obj, libzmq:libzmq.olb/lib
$ linkit remote_lat.obj,libzmq:libzmq.olb/lib
$ linkit remote_thr.obj,libzmq:libzmq.olb/lib
$!
$! Now define some "foreign commands" so the above programs
$! may be run.
$! Note:
$!  these commands are also defined by setEnv.com
$!
$ tlocal_lat :==$ "''def'local_lat.exe
$ tlocal_thr :==$ "''def'local_thr.exe
$ tremote_lat:==$ "''def'remote_lat.exe
$ tremote_thr:==$ "''remote_thr.exe
$!
$ purge/nolog                   !get rid of old versions
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ Exit
