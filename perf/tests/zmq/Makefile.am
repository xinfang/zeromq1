INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

noinst_PROGRAMS = local_lat remote_lat local_thr remote_thr \
local_fo remote_fo local_fi remote_fi local_thr_lat remote_thr_lat

local_thr_SOURCES = local_thr.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp \
../scenarios/thr.hpp

remote_thr_SOURCES = remote_thr.cpp ../../transports/zmq.hpp \
../scenarios/thr.hpp ../../transports/i_transport.hpp 

local_lat_SOURCES = local_lat.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp \
../scenarios/lat.hpp

remote_lat_SOURCES = remote_lat.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp ../scenarios/lat.hpp 

local_fo_SOURCES = local_fo.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp ../scenarios/fo.hpp

remote_fo_SOURCES = remote_fo.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp ../scenarios/fo.hpp

local_fi_SOURCES = local_fi.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp ../scenarios/fi.hpp

remote_fi_SOURCES = remote_fi.cpp ../../transports/zmq.hpp \
../../transports/i_transport.hpp ../scenarios/fi.hpp

local_thr_lat_SOURCES = local_thr_lat.cpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../helpers/functions.hpp \
../../transports/i_transport.hpp ../scenarios/thr_lat.hpp

remote_thr_lat_SOURCES = remote_thr_lat.cpp ../../transports/zmq.hpp \
../../helpers/time.hpp ../../helpers/functions.hpp \
../../transports/i_transport.hpp ../scenarios/thr_lat.hpp

local_thr_LDADD = $(top_builddir)/zmq/libzmq.la
local_thr_CXXFLAGS = -Wall -pedantic -DPERF_FORCE_GETTIMEOFDAY

remote_thr_LDADD = $(top_builddir)/zmq/libzmq.la
remote_thr_CXXFLAGS = -Wall -pedantic

local_lat_LDADD = $(top_builddir)/zmq/libzmq.la
local_lat_CXXFLAGS = -Wall -pedantic

remote_lat_LDADD = $(top_builddir)/zmq/libzmq.la
remote_lat_CXXFLAGS = -Wall -pedantic

local_fo_LDADD = $(top_builddir)/zmq/libzmq.la
local_fo_CXXFLAGS = -Wall -pedantic

remote_fo_LDADD = $(top_builddir)/zmq/libzmq.la
remote_fo_CXXFLAGS = -Wall -pedantic -DPERF_FORCE_GETTIMEOFDAY

local_fi_LDADD = $(top_builddir)/zmq/libzmq.la
local_fi_CXXFLAGS = -Wall -pedantic -DPERF_FORCE_GETTIMEOFDAY

remote_fi_LDADD = $(top_builddir)/zmq/libzmq.la
remote_fi_CXXFLAGS = -Wall -pedantic

local_thr_lat_LDADD = $(top_builddir)/zmq/libzmq.la
local_thr_lat_CXXFLAGS = -Wall -pedantic

remote_thr_lat_LDADD = $(top_builddir)/zmq/libzmq.la
remote_thr_lat_CXXFLAGS = -Wall -pedantic

