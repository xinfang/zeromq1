project(libzmq)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/zmq/platform.hpp.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/zmq/platform.hpp"
)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
)

set(libzmq_headers
 zmq.hpp
  zmq/amqp_decoder.hpp
  zmq/amqp_encoder.hpp
  zmq/amqp_marshaller.hpp
  zmq/amqp_client.hpp
  zmq/amqp_unmarshaller.hpp
  zmq/api_thread.hpp
  zmq/atomic_bitmap.hpp
  zmq/atomic_counter.hpp
  zmq/atomic_ptr.hpp
  zmq/bp_decoder.hpp
  zmq/bp_encoder.hpp
  zmq/bp_pgm_receiver.hpp
  zmq/bp_pgm_sender.hpp
  zmq/bp_tcp_engine.hpp
  zmq/bp_tcp_listener.hpp
  zmq/command.hpp
  zmq/config.hpp
  zmq/decoder.hpp
  zmq/i_demux.hpp
  zmq/publisher.hpp
  zmq/load_balancer.hpp
  zmq/devpoll_thread.hpp
  zmq/dispatcher.hpp
  zmq/encoder.hpp
  zmq/engine_base.hpp
  zmq/engine_factory.hpp
  zmq/epoll_thread.hpp
  zmq/err.hpp
  zmq/export.hpp
  zmq/fd.hpp
  zmq/formatting.hpp
  zmq/i_amqp.hpp
  zmq/i_engine.hpp
  zmq/i_locator.hpp
  zmq/in_engine.hpp
  zmq/io_thread.hpp
  zmq/ip.hpp
  zmq/i_pollable.hpp
  zmq/i_poller.hpp
  zmq/i_signaler.hpp
  zmq/i_thread.hpp
  zmq/kqueue_thread.hpp
  zmq/locator.hpp
  zmq/message.hpp
  zmq/mutex.hpp
  zmq/mux.hpp
  zmq/out_engine.hpp
  zmq/pgm_socket.hpp
  zmq/pipe.hpp
  zmq/poller.hpp
  zmq/poll_thread.hpp
  zmq/raw_message.hpp
  zmq/scope.hpp
  zmq/select_thread.hpp
  zmq/server_protocol.hpp
  zmq/stdint.hpp
  zmq/tcp_listener.hpp
  zmq/tcp_socket.hpp
  zmq/thread.hpp
  zmq/windows.hpp
  zmq/wire.hpp
  zmq/ypipe.hpp
  zmq/ypollset.hpp
  zmq/yqueue.hpp
  zmq/ysemaphore.hpp
  zmq/ysocketpair.hpp
  zmq/sctp_engine.hpp
  zmq/sctp_listener.hpp
  zmq/xmlParser.hpp
  zmq/data_dam.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/zmq/platform.hpp
)

set(libzmq_sources
  ${libzmq_headers}

  amqp_decoder.cpp
  amqp_encoder.cpp
  amqp_marshaller.cpp
  amqp_client.cpp
  amqp_unmarshaller.cpp
  api_thread.cpp
  bp_decoder.cpp
  bp_encoder.cpp
  bp_pgm_receiver.cpp
  bp_pgm_sender.cpp
  bp_tcp_engine.cpp
  bp_tcp_listener.cpp
  publisher.cpp
  load_balancer.cpp
  devpoll_thread.cpp
  dispatcher.cpp
  engine_factory.cpp
  epoll_thread.cpp
  err.cpp
  in_engine.cpp
  ip.cpp
  kqueue_thread.cpp
  locator.cpp
  mux.cpp
  out_engine.cpp
  pgm_socket.cpp
  pipe.cpp
  poll_thread.cpp
  select_thread.cpp
  tcp_listener.cpp
  tcp_socket.cpp
  thread.cpp
  ypollset.cpp
  ysemaphore.cpp
  ysocketpair.cpp
  sctp_engine.cpp
  sctp_listener.cpp
  xmlParser.cpp
  data_dam.cpp
)

set(libzmq_libraries
  ${ZMQ_SOCKET_LIBRARIES} 
  ${ZMQ_PTHREAD_LIBRARIES} 
)

IF(ZMQ_HAVE_SCTP)
  set(libzmq_libraries
    ${libzmq_libraries}
    ${LIBSCTP_LIBRARIES}
  )
ENDIF(ZMQ_HAVE_SCTP)

IF(ZMQ_HAVE_OPENPGM)
  LINK_LIBRARIES ("pgm-pic" ${GLIB2_LIBRARIES})
  INCLUDE_DIRECTORIES (${GLIB2_INCLUDE_DIRS})
ENDIF(ZMQ_HAVE_OPENPGM)

IF(ZMQ_HAVE_WINDOWS)
  add_definitions(-DZMQ_BUILDING_LIBZMQ -D_USE_XMLPARSER_DLL -D_DLL_EXPORTS_)
ENDIF(ZMQ_HAVE_WINDOWS)

zmq_add_library(zmq ${libzmq_sources})
target_link_libraries( zmq ${libzmq_libraries})
