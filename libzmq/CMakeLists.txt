project(libzmq)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/zmq/platform.hpp.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/zmq/platform.hpp"
)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
)

# Possibly add headers to this list if M$ IDE being used
set(libzmq_sources
  api_thread.cpp
  bp_decoder.cpp 
  bp_encoder.cpp 
  bp_tcp_engine.cpp
  bp_tcp_listener.cpp
  demux.cpp 
  devpoll_thread.cpp 
  dispatcher.cpp
  epoll_thread.cpp 
  err.cpp
  in_engine.cpp
  ip.cpp
  kqueue_thread.cpp
  locator.cpp
  mux.cpp
  out_engine.cpp
  pipe.cpp 
  pollable_factory.cpp
  poll_thread.cpp
  select_thread.cpp
  tcp_listener.cpp 
  tcp_socket.cpp
  thread.cpp
  ypollset.cpp
  ysemaphore.cpp
  ysocketpair.cpp
)

set(libzmq_libraries
  ${ZMQ_SOCKET_LIBRARIES} 
  ${ZMQ_PTHREAD_LIBRARIES} 
)

IF(ZMQ_HAVE_SCTP)
  set(libzmq_sources
    ${libzmq_sources}
    bp_sctp_engine.cpp
    bp_sctp_listener.cpp
  )
  set(libzmq_libraries
    ${libzmq_libraries}
    ${LIBSCTP_LIBRARIES}
  )
ENDIF(ZMQ_HAVE_SCTP)

zmq_add_library(zmq ${libzmq_sources})
target_link_libraries( zmq ${libzmq_libraries})
