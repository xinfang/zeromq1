project(zmq_tests)

include_directories(
  "${zmq_SOURCE_DIR}/libzmq"
  "${zmq_BINARY_DIR}/libzmq" # needed for generated platform.hpp
)

set(local_lat_sources 
  local_lat.cpp
)
add_executable(local_lat ${local_lat_sources})
target_link_libraries(local_lat zmq)

set(remote_lat_sources 
  remote_lat.cpp
)
add_executable(remote_lat ${remote_lat_sources})
target_link_libraries(remote_lat zmq)

set(local_thr_sources 
  local_thr.cpp
)
add_executable(local_thr ${local_thr_sources})
target_link_libraries(local_thr zmq)

set(remote_thr_sources 
  remote_thr.cpp
)
add_executable(remote_thr ${remote_thr_sources})
target_link_libraries(remote_thr zmq)

