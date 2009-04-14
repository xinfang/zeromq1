$! build_33_libjzmq.com
$! Created 2009-02-26
$! Modified
$!  2009-04-13 BRC - Changed optimization to TUNE=HOST (platform-agnostic)
$!+
$! Build the API for Java
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES
$!
$ @sys$startup:java$150_setup		! Set up the Java environment
$!
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqRoot:[libjzmq]		! ..where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$! Do the java first as the output from javah is required in the
$! C++ compile.
$!
$ javac Zmq.java
$ javah -jni -force -classpath [] Zmq
$!
$ write sys$output ""
$ write sys$output -
   "ZMQ - Ignore possible '%CXX-W-CONPTRLOSBIT, conversion from integer to smaller pointer' message"
$ write sys$output ""
$ cxx   /define=__USE_STD_IOSTREAM		-
	/prefix=all/float=ieee/ieee=denorm	-
	/names=(as_is,shortened)		-
	/OPTIMIZE=(INLINE=SPEED,LEVEL=4,UNROLL=0,TUNE=HOST) -
	/INCLUDE=(zmq:, libzmq:, java_include:) -
	Zmq.cpp
$!
$ write sys$output ""
$ write sys$output -
   "ZMQ - Ignore possible '%ILINK-W-COMPWARN, compilation warnings' message"
$ write sys$output ""
$ link /share=[]zmq.exe zmq.obj, sys$input/opt
libzmq:libzmq.olb/lib
!
GSMATCH=LEQUAL,1,1
!
case_sensitive=YES
SYMBOL_VECTOR = ( -
	Java_Jzmq_createExchange = PROCEDURE, -
	Java_Jzmq_bind           = PROCEDURE, -
	Java_Jzmq_send           = PROCEDURE, -
	Java_Jzmq_receive        = PROCEDURE, -
	Java_Jzmq_construct      = PROCEDURE, -
	Java_Jzmq_createQueue    = PROCEDURE, -
	Java_Jzmq_finalize       = PROCEDURE  -
        )
$!
$ purge/nolog [...]
$ ren [...]*.* [...]*.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
