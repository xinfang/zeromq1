$! BUILD_99_EXAMPLES_CHAT.COM
$! 2009-05-06
$! Modified:
$!    2009-05-06 ja - initial version
$!+
$! Build the chat example
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"	! save default
$ set default zmqRoot:[examples.chat]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit chatroom.cpp
$ compit display.cpp
$ compit prompt.cpp
$
$!
$ linkit chatroom.obj, libzmq:libzmq.olb/lib
$ linkit display.obj, libzmq:libzmq.olb/lib
$ linkit prompt.obj, libzmq:libzmq.olb/lib
$!
$ chat:==$ZMQROOT:[examples.chat]chatroom
$ disp:==$ZMQROOT:[examples.chat]display
$ prom:==$ZMQROOT:[examples.chat]prompt
$!
$ purge/nolog
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
