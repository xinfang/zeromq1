$! BUILD_99_EXAMPLES_BUTTERFLY.COM
$! 2009-05-06
$! Modified:
$!    2009-05-06 ja - initial version
$!+
$! Build the butterfly example
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"	! save default
$ set default zmqRoot:[examples.butterfly]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit component1.cpp
$ compit component2.cpp
$ compit intermediate.cpp
$ compit receive_replies.cpp
$ compit send_requests.cpp
$!
$ linkit component1.obj, libzmq:libzmq/lib
$ linkit component2.obj, libzmq:libzmq/lib
$ linkit intermediate.obj, libzmq:libzmq/lib
$ linkit receive_replies.obj, libzmq:libzmq/lib
$ linkit send_requests.obj, libzmq:libzmq/lib
$!
$ comp1:==$ZMQROOT:[examples.butterfly]component1
$ comp2:==$ZMQROOT:[examples.butterfly]component2
$ inter:==$ZMQROOT:[examples.butterfly]intermediate
$ recei:==$ZMQROOT:[examples.butterfly]receive_replies
$ sendr:==$ZMQROOT:[examples.butterfly]send_requests
$!
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
