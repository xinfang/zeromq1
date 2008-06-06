INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

noinst_PROGRAMS = local_lat remote_lat local_thr remote_thr

local_thr_SOURCES = local_thr.cpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../transports/i_transport.hpp \
../scenarios/thr.hpp

remote_thr_SOURCES = remote_thr.cpp ../../transports/zmq.hpp \
../scenarios/thr.hpp ../../transports/i_transport.hpp 

local_lat_SOURCES = local_lat.cpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../transports/i_transport.hpp \
../scenarios/lat.hpp

remote_lat_SOURCES = remote_lat.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp ../scenarios/lat.hpp 

local_thr_LDADD = $(top_builddir)/zmq/libzmq.la
local_thr_CXXFLAGS = -Wall -pedantic
remote_thr_LDADD = $(top_builddir)/zmq/libzmq.la
remote_thr_CXXFLAGS = -Wall -pedantic

local_lat_LDADD = $(top_builddir)/zmq/libzmq.la
local_lat_CXXFLAGS = -Wall -pedantic
remote_lat_LDADD = $(top_builddir)/zmq/libzmq.la
remote_lat_CXXFLAGS = -Wall -pedantic

