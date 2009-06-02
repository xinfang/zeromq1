$! BUILD_6_TESTS_JZMQ.COM
$! 2009-0518
$! Modified:
$!
$!+
$! Build the Java ZMQ test programs.
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"	! save default
$ set default zmqRoot:[perf.tests.zmq]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ define java$classpath libjzmq:, []	! libjzmq: is where zmq.java lives
$!
$ javac j_local_lat.java
$ javac j_remote_lat.java
$ javac j_local_thr.java
$ javac j_remote_thr.java
$!
$ deassign java$classpath
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
