INCLUDES = -I$(top_builddir)/libzmq -I$(top_srcdir)/libzmq

noinst_PROGRAMS = local_lat remote_lat

local_lat_SOURCES = local_lat.cpp ../../transports/qpid_transport.hpp  \
../../transports/i_transport.hpp ../scenarios/lat.hpp
local_lat_CXXFLAGS = -Wall -pedantic -Werror
local_lat_LDADD = -lqpidclient

remote_lat_SOURCES = remote_lat.cpp ../../transports/qpid_transport.hpp \
../../transports/i_transport.hpp ../scenarios/lat.hpp
remote_lat_CXXFLAGS = -Wall -pedantic -Werror
remote_lat_LDADD = -lqpidclient

