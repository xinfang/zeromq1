$! ZMQSETENV.COM
$! Created: 2009-02-17
$! Modified
$!  2009-03-10 ja - change zmqRoot to include the base level
$!  2009-03-12 ja - check for Java
$!  2009-03-25 pm - dir names to lower case
$!  2009-03-30 pm - changed include dirs for libcmzq and lijzmq
$!  2009-04-07 pm - added homeDir variable
$!  2009-05-04 ja - added homeDisk variable and moved the java_include
$!                  logical name into the OPEN/READ section
$! Use this to set the environment for a particular build version, e.g.,
$! zmq-dev
$!-
$ baseLevel == "zmq-dev" 	! change this where required
$!
$ if p1 .eqs. ""
$ then
$	write sys$output "No baselevel given, using ''baseLevel'"
$ else
$ 	baseLevel == "''p1'"
$ endif
$!
$ write sys$output "You are using base level ''baseLevel'"
$!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
$! Change the following line to reflect your home directory.
$!------------------------------------------------------------
$ homeDir == f$parse("sys$login",,,,"NO_CONCEAL")
$!
$! Get rid of the trailing junk
$ homeDir == ''homeDir' - "].;"
$!
$!
$ CALL setUPODS5	! set up for ODS-5 disk
$!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
$! This should now reflect the device and directory
$! of the installation.
$!------------------------------------------------------------
$ define/nolog/translation=(terminal,concealed)/job zmqRoot -
	"''homeDir'.''baseLevel'.]"
$!
$ write sys$output "zmqRoot defined to ''f$trnlnm("zmqRoot")'"
$!
$!
$! If Java 5 is on the machine, let's use it
$! 
$ open/read/error=noJava java_file sys$startup:java$150_setup.com
$!
$    @sys$startup:java$150_setup	! get Java first
$!
$!   Do some juggling in order to get to the include directories for Java
$!
$    part1 = "''f$trnlnm("JAVA$JRE_HOME_VMS")'" - ".jre]" + ".include]"
$    part2 = "''f$trnlnm("JAVA$JRE_HOME_VMS")'" - ".jre]" + ".include.vms]"
$!
$    def/job java_include  "''part1'", "''part2'" ! has the Java include dirs
$ close java_file
$!
$ noJava:
$!
$ def/job zmqBase	zmqRoot:[000000]    ! home...
$ def/job libzmq	zmqRoot:[libzmq]    ! is where the main OLB is
$ def/job libczmq	zmqRoot:[libczmq]   ! is where the C OLB is
$ def/job libjzmq	zmqRoot:[libjzmq]   ! is where Java code lives
$ def/job libvmszmq	zmqRoot:[libvmszmq] ! is where OpenVMS wrapper lives
$
$ def/job zmq   	zmqRoot:[openvms],   - ! where the include
			zmqRoot:[libzmq.zmq],- ! files are
	     		zmqRoot:[libczmq],   - ! C API
			zmqRoot:[libjzmq]      ! Java
$
$ def/job zmqOpenVMS	zmqRoot:[openvms]	! saves typing...
$
$ def/job lnk$library   libzmq:libzmq.olb, -	!Tell the linker
			libczmq:libczmq.olb	! .. where the LIBs are
$ def/job vmszmq	libvmszmq:vmszmq.exe	! Wrapper image
$
$ def/job zmqExamples	zmqRoot:[examples]	! Lots of nice stuff
$ def/job zmqPerf	zmqRoot:[perf]		! Top perf dir
$ def/job zmqTestsZMQ	zmqRoot:[perf.tests.zmq]! ..ZMQ
$ def/job zmqTestsTCP	zmqRoot:[perf.tests.tcp]! ..TCP
$!
$! This COM defines DCL symbols for compiling and linking,
$! with and without debug.
$!
$ @zmqOpenVMS:BUILD_1_COMP_LINK_SWITCHES
$!
$! Set up some "foreign commands" so people can run things like
$! zmq_server
$!
$ zmq_server :== $zmqRoot:[zmq_server]zmq_server.exe
$!
$! TCP tests
$!
$ tlocal_lat  :== $zmqRoot:[perf.tests.tcp]local_lat.exe
$ tlocal_thr  :== $zmqRoot:[perf.tests.tcp]local_thr.exe
$ tremote_lat :== $zmqRoot:[perf.tests.tcp]remote_lat.exe
$ tremote_thr :== $zmqRoot:[perf.tests.tcp]remote_thr.exe
$!
$! ZMQ tests - the C++ versions
$!
$ zlocal_lat  :== $zmqRoot:[perf.tests.zmq]local_lat.exe
$ zlocal_thr  :== $zmqRoot:[perf.tests.zmq]local_thr.exe
$ zremote_lat :== $zmqRoot:[perf.tests.zmq]remote_lat.exe
$ zremote_thr :== $zmqRoot:[perf.tests.zmq]remote_thr.exe
$!
$! ZMQ tests - the C versions
$!
$ c_zlocal_lat  :== $zmqRoot:[perf.tests.zmq]c_local_lat.exe
$ c_zlocal_thr  :== $zmqRoot:[perf.tests.zmq]c_local_thr.exe
$ c_zremote_lat :== $zmqRoot:[perf.tests.zmq]c_remote_lat.exe
$ c_zremote_thr :== $zmqRoot:[perf.tests.zmq]c_remote_thr.exe
$!
$! ZMQ tests - the Fortran version
$! For running Fortran examples: define vmszmq zmqroot:[libvmszmq]vmszmq.exe
$!
$ f_zlocal_lat  :== $zmqRoot:[perf.tests.zmq]f_local_lat.exe
$ f_zremote_lat :== $zmqRoot:[perf.tests.zmq]f_remote_lat.exe
$ f_zremote_lat :== $zmqRoot:[perf.tests.zmq]f_remote_lat.exe
$ f_zremote_thr :== $zmqRoot:[perf.tests.zmq]f_remote_thr.exe
$!
$! ZMQ tests - the Cobol version
$! For running Cobol examples: define vmszmq zmqroot:[libvmszmq]vmszmq.exe
$!
$ cob_zlocal_lat  :== $zmqRoot:[perf.tests.zmq]cob_local_lat.exe
$ cob_zremote_lat :== $zmqRoot:[perf.tests.zmq]cob_remote_lat.exe
$!
$!
$! Go to where it all happens...
$!
$ set default zmqbase		! Go to base level home directory
$ Exit
$!
$ setUPODS5: SUBROUTINE
$!
$! Sub-routine to set up the various logical names for an ODS-5 disk
$!
$!
$ set process/parse=extend
$ set rms/ext=65535/blo=127/seq/buff=255/net=127
$!
$ DEFINE/JOB/NOLOG  DECC$ARGV_PARSE_STYLE       ENABLE
$ DEFINE/JOB/NOLOG  DECC$EFS_CASE_PRESERVE      ENABLE
$ DEFINE/JOB/NOLOG  DECC$EFS_CHARSET            ENABLE
$ DEFINE/JOB/NOLOG  DECC$EFS_CASE_SPECIAL       TRUE
$ DEFINE/JOB/NOLOG  DECC$ENABLE_TO_VMS_LOGNAME_CACHE    ENABLE
$ DEFINE/JOB/NOLOG  DECC$ENABLE_GETENV_CACHE    TRUE
$ DEFINE/JOB/NOLOG  DECC$FD_lOCKING             1
$ DEFINE/JOB/NOLOG  DECC$FILE_SHARING           TRUE
$ DEFINE/JOB/NOLOG  DECC$POSIX_SEEK_STREAM_FILE TRUE
$ DEFINE/JOB/NOLOG  DECC$READDIR_DROPDOTNOTYPE  ENABLE
$!
$ DEFINE/JOB/NOLOG  JAVA$DAEMONIZE_MAIN_THREAD TRUE
$ DEFINE/JOB/NOLOG  JAVA$DISABLE_CMDFILE_WHITESPACE_PARSING TRUE
$ DEFINE/JOB/NOLOG  JAVA$FORK_PIPE_STYLE 2
$ DEFINE/JOB/NOLOG  JAVA$FORK_SUPPORT_CHDIR TRUE
$ DEFINE/JOB/NOLOG  JAVA$FORK_MAILBOX_MESSAGES 8
$ DEFINE/JOB/NOLOG  JAVA$WAIT_FOR_CHILDREN 5
$
$ DEFINE/JOB/NOLOG  JAVA$CREATE_DIR_WITH_OWNER_DELETE TRUE
$ DEFINE/JOB/NOLOG  JAVA$DELETE_ALL_VERSIONS TRUE
$ DEFINE/JOB/NOLOG  JAVA$FILENAME_CONTROLS "8"
$ DEFINE/JOB/NOLOG  JAVA$RENAME_ALL_VERSIONS TRUE
$ DEFINE/JOB/NOLOG  JAVA$TIMED_READ_USE_QIO 1
$ DEFINE/JOB/NOLOG  JAVA$FILE_OPEN_MODE 3
$ DEFINE/JOB/NOLOG  JAVA$CACHING_INTERVAL 60
$ DEFINE/JOB/NOLOG  JAVA$CACHING_DIRECTORY TRUE
$!
$ ENDSSUBROUTINE	! endsetUPODS5
