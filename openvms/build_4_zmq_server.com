$! BUILD_4_ZMQ_SERVER.COM
$! 2009-02-17
$! Modified:
$!   2009-03-18 ja - change to reflect new zmqROOT definition
$!+
$! Build the ZMQ server
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! Standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default  zmqRoot:[zmq_server]     ! go to the libzmq directory
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit zmq_server.cpp
$
$ linkit zmq_server.obj, libzmq:libzmq.olb/lib
$!
$! Build a "foreign command" with which we can invoke
$! the zmq_server we just built. Since we are in the zmq_server
$! already, we can use the default directory held in "def" to
$! specify the device and directory name.
$! NOTE: this "foreign command is also defined in setEnv.com
$!
$ zmq_server:==$"''def'zmq_server.exe
$!
$ purge/nolog
$ ren *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
