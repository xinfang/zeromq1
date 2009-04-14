$! BUILD_3_LIBCZMQ.COM
$! 2009-02-17
$! Modified:
$!  2009-03-18 ja  - change to reflect new zmqRoot definition
$!  2009-03-30 pm  - cmzq.cpp changed to zmq.cpp
$!  2009-04-13 BRC - support for Alpha (ensure all objects get into library)
$!+
$! Build the API for C
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES
$!
$ cpu = "''f$getsyi("ARCH_NAME")'"
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqRoot:[libczmq]		! ..where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ compit zmq.cpp
$!
$ lib/create libczmq.olb
$ lib/repl/nolog libczmq.olb *.obj;
$ if "''cpu'" .eqs. "Alpha"
$ then
$    lib/repl/nolog libczmq.olb [.cxx_repository]*.obj;
$ endif
$!
$ purge/nolog [...]
$ ren *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
