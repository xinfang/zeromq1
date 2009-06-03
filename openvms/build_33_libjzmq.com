$! build_33_libjzmq.com
$! 2009-02-26
$! Modified:
$!  2009-05-05 ja - changed Jzmq to Zmq in the linker options
$!+
$! Build the API for Java
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES
$!
$ @sys$startup:java$150_setup		! Set up the Java environment
$!
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqRoot:[libjzmq.org.zmq]		! ..where the Zmq.java is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$! Do the java first as the output from javah is required in the
$! C++ compile.
$!
$ javac Zmq.java
$! 
$! Go back to zmqRoot:[libjzmq] directory
$!
$ set default zmqRoot:[libjzmq]         ! ..where headers should be generated
$ javah -jni -force -classpath [] org.zmq.Zmq
$ jar cfv Zmq.jar [.org.zmq]Zmq.class [.org.zmq]Zmq$InboundData.class
$!
$ write sys$output ""
$ write sys$output -
   "ZMQ - Ignore possible '%CXX-W-CONPTRLOSBIT, conversion from integer to smaller pointer' message"
$ write sys$output ""
$ cxx   /define=__USE_STD_IOSTREAM		-
	/warnings=disable=CONPTRLOSBIT		-
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
$ linkxxx /share=[]zmq.exe zmq.obj, sys$input/opt
libzmq:libzmq.olb/lib
!
GSMATCH=LEQUAL,1,1
!
case_sensitive=YES
SYMBOL_VECTOR = ( -
	Java_org_zmq_Zmq_createExchange = PROCEDURE, -
	Java_org_zmq_Zmq_bind           = PROCEDURE, -
	Java_org_zmq_Zmq_send           = PROCEDURE, -
	Java_org_zmq_Zmq_receive        = PROCEDURE, -
	Java_org_zmq_Zmq_construct      = PROCEDURE, -
	Java_org_zmq_Zmq_createQueue    = PROCEDURE, -
	Java_org_zmq_Zmq_finalize       = PROCEDURE  -
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
