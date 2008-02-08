TEST_MSG_COUNT_LATENCY = 50000
TEST_MSG_COUNT_THRPUT = 1000000
TEST_MSG_SIZE = 6
PORT_NUMBER = 5555

# --- do not edit below thie line -- (unles you know what you are doing) ------

INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

EXTRA_DIST = ../../workers/receiver.hpp ../../workers/sender.hpp \
../../workers/ping_pong.hpp ../../transports/sctp.hpp \
../../transports/ysuite.hpp

noinst_PROGRAMS = local remote

local_SOURCES = local.cpp ../../transports/tcp.hpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../workers/raw_receiver.hpp ../../workers/raw_ping_pong.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp
remote_SOURCES = remote.cpp ../../transports/tcp.hpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../workers/raw_sender.hpp ../../workers/echo.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp 

local_CPPFLAGS = -D'TEST_MSG_COUNT_LATENCY=${TEST_MSG_COUNT_LATENCY}' \
-D'TEST_MSG_COUNT_THRPUT=${TEST_MSG_COUNT_THRPUT}' -D'TEST_MSG_SIZE=${TEST_MSG_SIZE}' \
-D'PORT_NUMBER=${PORT_NUMBER}'

remote_CPPFLAGS = -D'TEST_MSG_COUNT_LATENCY=${TEST_MSG_COUNT_LATENCY}' \
-D'TEST_MSG_COUNT_THRPUT=${TEST_MSG_COUNT_THRPUT}' -D'TEST_MSG_SIZE=${TEST_MSG_SIZE}' \
-D'PORT_NUMBER=${PORT_NUMBER}'


local_LDADD = $(top_builddir)/zmq/libzmq.la
remote_LDADD = $(top_builddir)/zmq/libzmq.la

