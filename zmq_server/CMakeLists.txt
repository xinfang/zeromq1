project(zmq_server)

include_directories(
  "${zmq_SOURCE_DIR}/libzmq"
  "${zmq_BINARY_DIR}/libzmq" # needed for generated platform.hpp
)

set(zmq_server_sources 
  zmq_server.cpp
)

zmq_add_executable(zmq_server ${zmq_server_sources})
target_link_libraries(zmq_server zmq)
