$! BUILD_99_EXAMPLES_EXCHANGE.COM
$! 2009-05-06
$! Modified:
$!    2009-05-06 ja - initial version
$!+
$! Build the exchange example
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES! standard compiler switches
$!
$ def="''f$environment("DEFAULT")'"	   ! save default
$ set default zmqRoot:[examples.exchange]  ! where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$! Need this until I work out how to get C++ to include properly
$!
$ copy/nolog zmqRoot:[perf.helpers]time.hpp []*
$!
$ compit me.cpp
$ compit gtw.cpp
$ compit stat.cpp
$!
$ linkit me.obj, libzmq:libzmq/lib
$ linkit gtw.obj, libzmq:libzmq/lib
$ linkit stat.obj, libzmq:libzmq/lib
$!
$ purge/nolog
$!
$! Delete time.hpp as it was only required for the compile
$!
$ delete/nolog/noconf []time.hpp.*
$!
$ gtw:==$ZMQROOT:[examples.exchange]gtw
$ me:==$ZMQROOT:[examples.exchange]me
$ stat:==$ZMQROOT:[examples.exchange]stat
$!
$ rename *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
