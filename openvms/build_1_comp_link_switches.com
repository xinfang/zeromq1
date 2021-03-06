$! BUILD_1_COMP_LINK_SWITCHES.COM
$! 2009-02-21
$! Modified:
$!  2009-05-05 ja - change ITANIUM to HOST on the TUNE parameter (portable)
$!                  added FLOAT=IEEE (better for I64)
$!                  added COBOL
$!+
$! Set the switches to be used for the C++ Compiler and
$! define a symbol for the compiler.
$! Set the switches to be used by the link command and
$! define a symbol for the link command.
$! All build files should invoke ( $ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES )
$! this file prior to compiling and linking.
$!-
$ compit :== "cxx/define=__USE_STD_IOSTREAM/names=(as_is,shortened) /OPTIMIZE=(INLINE=SPEED,LEVEL=4,UNROLL=0,TUNE=HOST) /FLOAT=IEEE/REENTRANCY=MULTITHREAD /INCLUDE=(ZMQ:, LIBZMQ:)"
$ compitd:== "cxx/debug=all/noop/define=__USE_STD_IOSTREAM/names=(as_is,shortened) /INCLUDE=(ZMQ:, LIBZMQ:)"
$ ccompit :== "cc/optimize=(level=5,inline=all)/reentrancy=multithread /include=(zmq:,libczmq:,libzmq:) /name=as_is
$ ccompitd:== "cc/debug=all/noop/define=__USE_STD_IOSTREAM/names=(as_is,shortened) /INCLUDE=(ZMQ:, LIBZMQ:)"
$ cobit   :== "cobol/ARCHITECTURE=HOST /OPTIMIZE=(LEVEL=4,TUNE=HOST) /FLOAT=IEEE"
$ forit   :== "fortran/extend_source/REENTRANCY=threaded /OPTIMIZE=(INLINE=ALL,LEVEL=5,TUNE=HOST) /FLOAT=IEEE"
$ linkit :== "linkxxx /THREADS_ENABLE=(MULTIPLE_KERNEL_THREADS,UPCALLS)"
$ linkitd:== "linkxxx/debug"
$!
