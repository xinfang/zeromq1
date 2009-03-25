$! run_tests_zmq.com
$! 2009-02-19
$!+
$! Set up the modules for testing
$! Note: the following foreign commands are defined in setEnv.com
$!-
$ def = "''f$environment("DEFAULT")'"   ! save default
$ set default zmqBase                   ! start from known location
$ set default [.perf.tests.zmq]         ! where the source is
$ defit = "''f$environment("DEFAULT")'" ! save default
$ write sys$output "Running in ''f$environment("DEFAULT")'"
$!
$! This is the C++ version of the latency tests.
$!
$ zmq_server
$ zlocal_lat 127.0.0.1 127.0.0.1:5555 127.0.0.1:5556 64 10000
$ zremote_lat 127.0.0.1 64 10000
$!
$! C++ versions of the throughput tests.
$!
$ zmq_server
$ zlocal_thr 127.0.0.1 127.0.0.1:5555 127.0.0.1:5556 64 1000000 1
$ zremote_thr 127.0.0.1 64 1000000 1
$!
$! The C++ version of the latency TCP tests
$!
$ tlocal_lat 127.0.0.1:5555 64 10000
$ tremote_lat 127.0.0.1:5555 64 10000
$!
$! The C++ version of the throughput TCP tests
$!
$ tlocal_thr 127.0.0.1:5555 64 10000 1
$ tremote_thr 127.0.0.1:5555 64 10000 1
$!
$! This is the C program running the latency tests.
$!
$ zmq_server
$ c_zremote_lat 127.0.0.1 127.0.0.1:5555 127.0.0.1:5556 64 100000
$ c_zlocal_lat 127.0.0.1 64 100000
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ Exit
