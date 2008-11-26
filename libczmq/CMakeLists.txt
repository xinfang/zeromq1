if(WITH_C)

project(libczmq)

include_directories(
    "${zmq_SOURCE_DIR}/libzmq"
    "${zmq_BINARY_DIR}/libzmq"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

zmq_add_library(czmq czmq.cpp)

endif(WITH_C)
