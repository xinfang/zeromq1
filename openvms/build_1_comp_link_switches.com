$! BUILD_1_COMP_LINK_SWITCHES.COM
$! 2009-02-21
$!+
$! Set the switches to be used for the C++ Compiler and
$! define a symbol for the compiler.
$! Set the switches to be used by the link command and
$! define a symbol for the link command.
$! All build files should invoke ( $ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES )
$! this file prior to compiling and linking.
$!-
$ compit :== "cxx/define=__USE_STD_IOSTREAM/names=(as_is,shortened) /OPTIMIZE=(INLINE=SPEED,LEVEL=4,UNROLL=0,TUNE=ITANIUM) /REENTRANCY=MULTITHREAD /INCLUDE=(ZMQ:, LIBZMQ:)"
$ compitd:== "cxx/debug=all/noop/define=__USE_STD_IOSTREAM/names=(as_is,shortened) /INCLUDE=(ZMQ:, LIBZMQ:)"
$ ccompit :== "cc/optimize=(level=5,inline=all)/reentrancy=multithread /include=(zmq:,libczmq:,libzmq:) /name=as_is
$ ccompitd:== "cc/debug=all/noop/define=__USE_STD_IOSTREAM/names=(as_is,shortened) /INCLUDE=(ZMQ:, LIBZMQ:)"
$ forit   :== "fortran/extend_source/REENTRANCY=threaded"
$ linkit :== "linkxxx /THREADS_ENABLE=(MULTIPLE_KERNEL_THREADS,UPCALLS)"
$ linkitd:== "linkxxx/debug"
$!
