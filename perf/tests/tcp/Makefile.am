INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

EXTRA_DIST = ../perf/workers/receiver.hpp ../perf/workers/sender.hpp \
../perf/workers/ping_pong.hpp ../perf/transports/sctp.hpp \
../perf/transports/ysuite.hpp

noinst_PROGRAMS = local_thr remote_thr local_lat remote_lat

local_thr_SOURCES = local_thr.cpp test.hpp ../../transports/tcp.hpp \
../../helpers/time.hpp ../../helpers/files.hpp ../../workers/raw_receiver.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp
remote_thr_SOURCES = remote_thr.cpp test.hpp ../../transports/tcp.hpp \
../../helpers/time.hpp ../../workers/raw_sender.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp 
local_lat_SOURCES = local_lat.cpp test.hpp ../../transports/tcp.hpp \
../../helpers/time.hpp ../../helpers/files.hpp ../../workers/ping_pong.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp
remote_lat_SOURCES = remote_lat.cpp test.hpp ../../transports/tcp.hpp \
../../helpers/time.hpp ../../helpers/files.hpp ../../workers/echo.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp 

