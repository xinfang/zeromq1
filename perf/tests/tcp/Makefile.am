INCLUDES = -I$(top_builddir) -I$(top_srcdir)  -I$(top_builddir)/libzmq \
-I$(top_srcdir)/libzmq

noinst_PROGRAMS = local_thr remote_thr local_lat remote_lat

local_thr_SOURCES = local_thr.cpp ../../transports/tcp_transport.hpp \
../../transports/i_transport.hpp ../scenarios/thr.hpp
local_thr_CXXFLAGS = -Wall -pedantic -Werror
local_thr_LDADD = $(top_builddir)/libzmq/libzmq.la 

remote_thr_SOURCES = remote_thr.cpp ../../transports/tcp_transport.hpp \
../../transports/i_transport.hpp ../scenarios/thr.hpp 
remote_thr_CXXFLAGS = -Wall -pedantic -Werror
remote_thr_LDADD = $(top_builddir)/libzmq/libzmq.la

local_lat_SOURCES = local_lat.cpp ../../transports/tcp_transport.hpp  \
../../transports/i_transport.hpp ../scenarios/lat.hpp
local_lat_CXXFLAGS = -Wall -pedantic -Werror
local_lat_LDADD = $(top_builddir)/libzmq/libzmq.la

remote_lat_SOURCES = remote_lat.cpp ../../transports/tcp_transport.hpp \
../../transports/i_transport.hpp ../scenarios/lat.hpp
remote_lat_CXXFLAGS = -Wall -pedantic -Werror
remote_lat_LDADD = $(top_builddir)/libzmq/libzmq.la

