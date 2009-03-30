$! BUILD_1_ALL.COM
$!+
$! 2009-03-30
$!  Modified:
$! Compiles all modules required for 0MQ
$!-
$ write sys$output "-------------------------------------"
$ Write Sys$output "Beginning compilation of all 0MQ modules at ''f$time()'"
$ write sys$output "-------------------------------------"
$!
$@ZMQROOT:[openvms]build_2_libzmq
$@ZMQROOT:[openvms]build_33_libjzmq
$@ZMQROOT:[openvms]build_3_libczmq
$@ZMQROOT:[openvms]build_41_libopenvms
$@ZMQROOT:[openvms]build_4_zmq_server
$@ZMQROOT:[openvms]build_5_tests_zmq
$! @ZMQROOT:[openvms]build_61_tests_forzmq
$! @ZMQROOT:[openvms]build_61_tests_fzmq
$@ZMQROOT:[openvms]build_6_tests_czmq
$@ZMQROOT:[openvms]build_7_tests_tcp
$!
$ write sys$output "-------------------------------------"
$ Write Sys$output "Compiled all 0MQ modules at ''f$time()'"
$ write sys$output "-------------------------------------"
$ Exit

