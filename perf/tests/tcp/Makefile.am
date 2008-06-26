noinst_PROGRAMS = local_thr remote_thr

local_thr_SOURCES = local_thr.cpp ../../transports/tcp.hpp \
../../helpers/time.hpp ../../transports/i_transport.hpp \
../scenarios/thr.hpp

remote_thr_SOURCES = remote_thr.cpp ../../transports/tcp.hpp \
../../helpers/time.hpp ../../transports/i_transport.hpp \
../scenarios/thr.hpp 

local_thr_CXXFLAGS = -Wall -pedantic -DPERF_FORCE_GETTIMEOFDAY

remote_thr_CXXFLAGS = -Wall -pedantic

#local_lat_SOURCES = local_lat.cpp ../../transports/tcp.hpp ../../helpers/files.hpp \
#../../helpers/time.hpp ../../workers/raw_ping_pong.hpp \
#../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp test.hpp

#remote_lat_SOURCES = remote_lat.cpp ../../transports/tcp.hpp test.hpp \
#../../workers/echo.hpp ../../interfaces/i_transport.hpp \
#../../interfaces/i_worker.hpp 

