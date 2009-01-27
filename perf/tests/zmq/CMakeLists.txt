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

if(ZMQ_HAVE_OPENPGM)
  set(pgm_remote_lat_sources 
    pgm_remote_lat.cpp
  )
  add_executable(pgm_remote_lat ${pgm_remote_lat_sources})
  target_link_libraries(pgm_remote_lat zmq)

  set(pgm_local_lat_sources 
    pgm_local_lat.cpp
  )
  add_executable(pgm_local_lat ${pgm_local_lat_sources})
  target_link_libraries(pgm_local_lat zmq)
endif(ZMQ_HAVE_OPENPGM)
