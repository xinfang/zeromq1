# Try to find Stream Control Transmission Protocol (SCTP) 
#
# The following variables will be set
#  LIBSCTP_FOUND
#  LIBSCTP_INCLUDE_DIRS 
#  LIBSCTP_LIBRARIES

include(LibFindMacros)

libfind_pkg_check_modules(LIBSCTP_PKGCONF LIBSCTP)

find_path(LIBSCTP_INCLUDE_DIR
  NAMES netinet/sctp.h
  PATHS ${SCTP_PKGCONF_INCLUDE_DIRS}
)

find_library(LIBSCTP_LIBRARY
  NAMES sctp
  PATHS ${LIBSCTP_PKGCONF_LIBRARY_DIRS}
)

set(LIBSCTP_PROCESS_INCLUDES LIBSCTP_INCLUDE_DIR )
set(LIBSCTP_PROCESS_LIBS     LIBSCTP_LIBRARY )

libfind_process(LIBSCTP)
