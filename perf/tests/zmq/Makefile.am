INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

noinst_PROGRAMS = local_thr remote_thr remote_lat local_lat \
local_thr_mt remote_thr_mt

local_thr_SOURCES = local_thr.cpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../workers/raw_receiver.hpp test.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp

local_thr_mt_SOURCES = local_thr_mt.cpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../workers/raw_receiver.hpp test.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp

remote_thr_SOURCES = remote_thr.cpp ../../transports/zmq.hpp \
../../workers/raw_receiver.hpp test.hpp ../../interfaces/i_transport.hpp \
../../interfaces/i_worker.hpp

remote_thr_mt_SOURCES = remote_thr_mt.cpp ../../transports/zmq.hpp \
../../workers/raw_receiver.hpp test.hpp ../../interfaces/i_transport.hpp \
../../interfaces/i_worker.hpp

local_lat_SOURCES = local_lat.cpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../workers/raw_ping_pong.hpp test.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp

remote_lat_SOURCES = remote_lat.cpp ../../transports/zmq.hpp \
../../workers/echo.hpp test.hpp ../../interfaces/i_transport.hpp \
../../interfaces/i_worker.hpp 

local_thr_LDADD = $(top_builddir)/zmq/libzmq.la
local_thr_mt_LDADD = $(top_builddir)/zmq/libzmq.la
remote_thr_LDADD = $(top_builddir)/zmq/libzmq.la
remote_thr_mt_LDADD = $(top_builddir)/zmq/libzmq.la

local_lat_LDADD = $(top_builddir)/zmq/libzmq.la
remote_lat_LDADD = $(top_builddir)/zmq/libzmq.la

