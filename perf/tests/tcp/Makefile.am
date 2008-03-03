noinst_PROGRAMS = local_thr remote_thr local_lat remote_lat

local_thr_SOURCES = local_thr.cpp ../../transports/tcp.hpp ../../helpers/files.hpp \
../../helpers/time.hpp ../../workers/raw_receiver.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp test.hpp

remote_thr_SOURCES = remote_thr.cpp ../../transports/tcp.hpp test.hpp \
../../workers/raw_sender.hpp ../../interfaces/i_transport.hpp \
../../interfaces/i_worker.hpp 

local_lat_SOURCES = local_lat.cpp ../../transports/tcp.hpp ../../helpers/files.hpp \
../../helpers/time.hpp ../../workers/raw_ping_pong.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp test.hpp

remote_lat_SOURCES = remote_lat.cpp ../../transports/tcp.hpp test.hpp \
../../workers/echo.hpp ../../interfaces/i_transport.hpp \
../../interfaces/i_worker.hpp 

