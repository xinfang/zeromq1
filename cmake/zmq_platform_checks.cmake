# -----------------------------------------------------------------------------
# Determine the platform we are building on
# -----------------------------------------------------------------------------

string(TOUPPER ${CMAKE_SYSTEM_NAME} ZMQ_SYSTEM_NAME)

if("${ZMQ_SYSTEM_NAME}" MATCHES "LINUX")
  set(ZMQ_HAVE_LINUX  1 CACHE INTERNAL "Building on Linux")
  set(ZMQ_PLATFORM_NAME "ZMQ_HAVE_LINUX")

elseif("${ZMQ_SYSTEM_NAME}" MATCHES "WINDOWS")
  set(ZMQ_HAVE_WINDOWS  1 CACHE INTERNAL "Building on Windows")
  set(ZMQ_PLATFORM_NAME "ZMQ_HAVE_WINDOWS")

elseif("${ZMQ_SYSTEM_NAME}" MATCHES "FREEBSD")
  set(ZMQ_HAVE_FREEBSD  1 CACHE INTERNAL "Building on FreeBSD")
  set(ZMQ_PLATFORM_NAME "ZMQ_HAVE_FREEBSD")

# not sure what cmake system name is for qnx
elseif("${ZMQ_SYSTEM_NAME}" MATCHES "QNX")
  set(ZMQ_HAVE_QNXNTO  1 CACHE INTERNAL "Building on QNX")
  set(ZMQ_PLATFORM_NAME "ZMQ_HAVE_QNXNTO")

elseif("${ZMQ_SYSTEM_NAME}" MATCHES "SUNOS")
  set(ZMQ_HAVE_SOLARIS  1 CACHE INTERNAL "Building on Solaris")
  set(ZMQ_PLATFORM_NAME "ZMQ_HAVE_SOLARIS")

elseif("${ZMQ_SYSTEM_NAME}" MATCHES "AIX")
  set(ZMQ_HAVE_AIX  1 CACHE INTERNAL "Building on AIX")
  set(ZMQ_PLATFORM_NAME "ZMQ_HAVE_AIX")

elseif("${ZMQ_SYSTEM_NAME}" MATCHES "HP-UX")
  set(ZMQ_HAVE_HPUX  1 CACHE INTERNAL "Building on HP/UX")
  set(ZMQ_PLATFORM_NAME "ZMQ_HAVE_HPUX")

# otherwise unknown platform 
else("${ZMQ_SYSTEM_NAME}" MATCHES "LINUX")
  message(FATAL_ERROR "ZMQ doesn't know this platform, unsupported")
endif("${ZMQ_SYSTEM_NAME}" MATCHES "LINUX")

message(STATUS "Determined build platform is ${ZMQ_PLATFORM_NAME}")

# -----------------------------------------------------------------------------
# find threads library
# -----------------------------------------------------------------------------

include(zmq_find_pthreads) # make this a find_package()

# -----------------------------------------------------------------------------
# find socket library, if any
# -----------------------------------------------------------------------------

include(zmq_find_socket) # make this a find_package()

# -----------------------------------------------------------------------------
# Stream Control Transmission Protocol (SCTP) functionality
# -----------------------------------------------------------------------------

if(WITH_SCTP)
  find_package(LIBSCTP REQUIRED)

  # find_package should fail if not found so there
  # shouldn't be a need to catch-gaurd this
  set(ZMQ_HAVE_SCTP 1)
endif(WITH_SCTP)

# -----------------------------------------------------------------------------
# Other platform specific checks here
# -----------------------------------------------------------------------------

# none yet.
