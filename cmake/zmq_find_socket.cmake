# - This module determines the socket libraries, if any, we need to link with
#
# The following variables are set
#  ZMQ_SOCKET_LIBRARIES    - the socket/nameserver libraries
#  ZMQ_SOCKET_DEFINES      - the socket definitions

include (CheckLibraryExists)
include (CheckFunctionExists)

set(ZMQ_SOCKET_LIB_FOUND)
set(ZMQ_NS_LIB_FOUND)
set(ZMQ_SOCKET_LIBRARIES)
set(ZMQ_SOCKET_DEFINES)

if(ZMQ_HAVE_WINDOWS)
  set(ZMQ_SOCKET_LIBRARIES ws2_32)
else(ZMQ_HAVE_WINDOWS)
  check_function_exists(setsockopt HAVE_SETSOCKOPT)

  if(NOT HAVE_SETSOCKOPT)
    SET(SOCKET_LIBS_TO_CHECK socket)

    foreach(lib $ENV{SOCKET_LIBS} ${SOCKET_LIBS_TO_CHECK})
      if(NOT ZMQ_SOCKET_LIB_FOUND)
        check_library_exists("${lib}" setsockopt "" ${lib}_FOUND)
        if(${lib}_FOUND)
          list(APPEND ZMQ_SOCKET_LIBRARIES ${lib})
          set(ZMQ_SOCKET_LIB_FOUND 1)
        endif(${lib}_FOUND)
      endif(NOT ZMQ_SOCKET_LIB_FOUND)
    endforeach(lib $ENV{SOCKET_LIBS} ${SOCKET_LIBS_TO_CHECK})
  endif(NOT HAVE_SETSOCKOPT)

  check_function_exists(gethostent HAVE_GETHOSTENT)

  if(NOT HAVE_GETHOSTENT)
    set(NS_LIBS_TO_CHECK nsl)

    foreach(lib ${NS_LIBS_TO_CHECK})
      if(NOT ZMQ_NS_LIB_FOUND)
        check_library_exists("${lib}" gethostent "" ${lib}_FOUND)
        if(${lib}_FOUND)
          list(APPEND ZMQ_SOCKET_LIBRARIES ${lib})
          set(ZMQ_NS_LIB_FOUND 1)
        endif(${lib}_FOUND)
      endif(NOT ZMQ_NS_LIB_FOUND)
    endforeach(lib ${NS_LIBS_TO_CHECK})
  endif(NOT HAVE_GETHOSTENT)
endif(ZMQ_HAVE_WINDOWS)

mark_as_advanced(ZMQ_SOCKET_LIBRARIES ZMQ_SOCKET_DEFINES) 

if(ZMQ_CMAKE_VERBOSE)
  message(STATUS "ZMQ_SOCKET_LIBRARIES   [${ZMQ_SOCKET_LIBRARIES}]")
  message(STATUS "ZMQ_SOCKET_DEFINES     [${ZMQ_SOCKET_DEFINES}]") 
endif(ZMQ_CMAKE_VERBOSE)
