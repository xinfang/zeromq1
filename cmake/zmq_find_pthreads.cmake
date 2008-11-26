# 2008.11.10, GNeill, this is a partial rewrite of cmake FindThreads.cmake 
#                     needs some work yet, but should work for most cases. 
#
# - This module determines the pthread library / compile definitions 
#
# The following variables are set
#  ZMQ_PTHREAD_LIBRARIES    - the thread libraries
#  ZMQ_PTHREAD_DEFINES      - the thread definitions

include(CheckLibraryExists)

set(ZMQ_PTHREADS_FOUND)
set(ZMQ_PTHREAD_LIBRARIES)
set(ZMQ_PTHREAD_DEFINES)

set(LIBS_TO_CHECK $ENV{PTHREAD_LIBS} pthreads pthread)

# let's look for libraries to link against
foreach(lib ${LIBS_TO_CHECK})
  if(NOT ZMQ_PTHREADS_FOUND)
    check_library_exists("${lib}" pthread_create "" ${lib}_FOUND)
    if(${lib}_FOUND)
      set(ZMQ_PTHREAD_LIBRARIES "${lib}")
      set(ZMQ_PTHREADS_FOUND 1)
    endif(${lib}_FOUND)
  endif(NOT ZMQ_PTHREADS_FOUND)
endforeach(lib ${LIBS_TO_CHECK})

if(NOT ZMQ_PTHREADS_FOUND)
  # let's look for compiler flags then
  set(FLAGS_TO_CHECK $ENV{PTHREAD_CXXFLAGS} -pthread)

  foreach(flag ${FLAGS_TO_CHECK})
    if(NOT ZMQ_PTHREADS_FOUND)
      message(STATUS "Check if compiler accepts ${flag}")

      try_run(THREADS_PTHREAD_ARG${flag} THREADS_HAVE_PTHREAD_ARG${flag}
          ${CMAKE_BINARY_DIR}
          ${CMAKE_ROOT}/Modules/CheckForPthreads.c
          CMAKE_FLAGS -DLINK_LIBRARIES:STRING=${flag}
          OUTPUT_VARIABLE OUTPUT)

      if(THREADS_HAVE_PTHREAD_ARG${flag})
        if(THREADS_PTHREAD_ARG${flag} MATCHES "^2$")
          set(ZMQ_PTHREAD_LIBRARIES "${flag}")
          set(ZMQ_PTHREADS_FOUND 1)
        endif(THREADS_PTHREAD_ARG${flag} MATCHES "^2$")
      endif(THREADS_HAVE_PTHREAD_ARG${flag})

      if(ZMQ_PTHREADS_FOUND)
        message(STATUS "Check if compiler accepts ${flag} - yes")
      else(ZMQ_PTHREADS_FOUND)
        message(STATUS "Check if compiler accepts ${flag} - no")
      endif(ZMQ_PTHREADS_FOUND)

    endif(NOT ZMQ_PTHREADS_FOUND)
  endforeach(flag ${FLAGS_TO_CHECK})
endif(NOT ZMQ_PTHREADS_FOUND)

# 2008.11.10, GNeill, check for pthread_create func, perhaps? 

# let's set compiler defines here 
if(ZMQ_PTHREADS_FOUND)
  if(ZMQ_HAVE_LINUX OR ZMQ_HAVE_SOLARIS)
    set(ZMQ_PTHREAD_DEFINES -D_REENTRANT)

  elseif(ZMQ_HAVE_FREEBSD ) # OR ZMQ_HAVE_QNXNTO)
    set(ZMQ_PTHREAD_DEFINES -D_THREAD_SAFE)

  # everything else
  else(ZMQ_HAVE_LINUX OR ZMQ_HAVE_SOLARIS)
    set(ZMQ_PTHREAD_DEFINES)
  endif(ZMQ_HAVE_LINUX OR ZMQ_HAVE_SOLARIS)
endif(ZMQ_PTHREADS_FOUND)

MARK_AS_ADVANCED(ZMQ_PTHREAD_LIBRARIES ZMQ_PTHREAD_DEFINES)

if(ZMQ_CMAKE_VERBOSE)
  message(STATUS "ZMQ_PTHREADS_FOUND      [${ZMQ_PTHREADS_FOUND}]")
  message(STATUS "ZMQ_PTHREAD_LIBRARIES   [${ZMQ_PTHREAD_LIBRARIES}]")
  message(STATUS "ZMQ_PTHREAD_DEFINES     [${ZMQ_PTHREAD_DEFINES}]") 
endif(ZMQ_CMAKE_VERBOSE)

