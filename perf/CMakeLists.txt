#  If WITH_PERF=YES descend into particular tests directories.
if (WITH_PERF)
    add_subdirectory ("tests/zmq") 
endif (WITH_PERF)

