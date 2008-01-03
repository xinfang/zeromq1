TEST_MSG_COUNT_LATENCY = 50000
TEST_MSG_COUNT_THRPUT = 1000000
TEST_MSG_SIZE = 6
PORT_NUMBER = 5555

# --- do not edit below thie line -- (unles you know what you are doing) ------

INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

bin_PROGRAMS = local remote

local_SOURCES = local.cpp ../perf/transports/tcp.hpp ../perf/transports/zmq.hpp \
../perf/helpers/time.hpp ../perf/workers/raw_receiver.hpp ../perf/workers/raw_ping_pong.hpp
remote_SOURCES = remote.cpp ../perf/transports/tcp.hpp ../perf/transports/zmq.hpp \
../perf/helpers/time.hpp ../perf/workers/raw_sender.hpp ../perf/workers/echo.hpp

local_CPPFLAGS = -D'TEST_MSG_COUNT_LATENCY=${TEST_MSG_COUNT_LATENCY}' \ 
-D'TEST_MSG_COUNT_THRPUT=${TEST_MSG_COUNT_THRPUT}' -D'TEST_MSG_SIZE=${TEST_MSG_SIZE}' \
-D'PORT_NUMBER=${PORT_NUMBER}'

remote_CPPFLAGS = -D'TEST_MSG_COUNT_LATENCY=${TEST_MSG_COUNT_LATENCY}' \
-D'TEST_MSG_COUNT_THRPUT=${TEST_MSG_COUNT_THRPUT}' -D'TEST_MSG_SIZE=${TEST_MSG_SIZE}' \
-D'PORT_NUMBER=${PORT_NUMBER}'


local_LDADD = $(top_builddir)/zmq/libzmq.a
remote_LDADD = $(top_builddir)/zmq/libzmq.a


