INCLUDES = -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/zmq -I$(top_srcdir)/zmq

bin_PROGRAMS = zmq_server
zmq_server_LDADD = $(top_builddir)/zmq/libzmq.la
zmq_server_SOURCES = zmq_server.cpp 
zmq_server_CXXFLAGS = -Wall -pedantic -Werror


