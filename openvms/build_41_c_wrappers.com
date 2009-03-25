$! BUILD_41_C_WRAPPERS.COM
$! 2009-03-24
$! Modified:
$!  
$!+
$! Build the C wrapper for high-level languages such as COBOL, FORTRAN
$!-
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES 
$!
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqRoot:[libczmq]		! ..where the source is
$ write sys$output "Building ''f$environment("DEFAULT")'"
$!
$ ccompit zmq_wrappers.c
$!
$ linkit/share=zmq_shr.exe zmq_wrappers, sys$input/opt
libczmq:libczmq.olb/lib
libzmq:libzmq.olb/lib
!
GSMATCH=LEQUAL,1,0
!
case_sensitive=yes
SYMBOL_VECTOR = ( -
	ZMQ_CREATE           = PROCEDURE, -
	ZMQ_DESTROY          = PROCEDURE, -
	ZMQ_CREATE_EXCHANGE  = PROCEDURE, -
	ZMQ_CREATE_QUEUE     = PROCEDURE, -
	ZMQ_BIND             = PROCEDURE, -
	ZMQ_SEND 	     = PROCEDURE, -
	ZMQ_RECEIVE          = PROCEDURE  -
)
!
! Also include entry points for the CZMQ functions (libczmq)
!
SYMBOL_VECTOR = ( -
 	czmq_create          = PROCEDURE, -
 	czmq_destroy	     = PROCEDURE, -
 	czmq_create_exchange = PROCEDURE, -
 	czmq_create_queue    = PROCEDURE, -
 	czmq_bind            = PROCEDURE, -
 	czmq_send            = PROCEDURE, -
 	czmq_receive         = PROCEDURE  -
)
!
$!
$ purge/nolog
$ ren *.* *.*;1
$ write sys$output "Built ''f$environment("DEFAULT")' at ''f$time()'"
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ exit
