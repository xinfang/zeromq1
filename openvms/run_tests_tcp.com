$! run_tests_tcp.com
$! 2009-02-19
$!+
$! Set up the modules for testing
$!-
$ def="''f$environment("DEFAULT")'"     ! save default
$ set default zmqBase                   ! start from known location
$ set default [.perf.tests.tcp]                ! where the source is
$ write sys$output "Running in ''f$environment("DEFAULT")'"
$!
$! Note: The commands are defined in setEnv.com
$!
$ define/user sys$output NL:
$ spawn/nowait tlocal_lat  127.0.0.1:5555 127 1000000
$ define/user sys$output NL:
$ spawn/nowait tremote_lat 127.0.0.1:5555 127 1000000
$
$ inquire more "Press <CR> to run next tests"
$ define/user sys$output NL:
$ spawn/nowait tlocal_thr  127.0.0.1:5555 127 1000000 1
$ define/user sys$output NL:
$ spawn/nowait tremote_thr 127.0.0.1:5555 127 1000000 1
$!
$!
$! Put the user back to the directory she started with
$!
$ set def 'def
$ Exit
