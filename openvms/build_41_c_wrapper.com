$! BUILD_3_LIBCZMQ.COM
$! 2009-02-17
$! Modified:
$!  2009-03-18 ja - change to reflect new zmqRoot definition
$!+
$! Build the API for C
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES 
$!
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqRoot:[libczmq]		! ..where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit czmq.cpp
$!
$ lib/create libczmq.olb
$ lib/repl/nolog libczmq.olb *.obj;
$!
$ purge/nolog
$ ren *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
