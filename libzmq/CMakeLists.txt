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
  bp_engine.cpp 
  bp_listener.cpp
  demux.cpp 
  devpoll_thread.cpp 
  dispatcher.cpp
  epoll_thread.cpp 
  err.cpp
  ip.cpp
  kqueue_thread.cpp
  locator.cpp
  mux.cpp
  pipe.cpp 
  poll_thread.cpp
  select_thread.cpp
  tcp_listener.cpp 
  tcp_socket.cpp
  thread.cpp
  ypollset.cpp
  ysemaphore.cpp
  ysocketpair.cpp
)

zmq_add_library(zmq ${libzmq_sources})

target_link_libraries(zmq 
  ${ZMQ_SOCKET_LIBRARIES} ${ZMQ_PTHREAD_LIBRARIES}
)
