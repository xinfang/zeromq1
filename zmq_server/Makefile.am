INCLUDES = -I$(top_builddir)/libzmq -I$(top_srcdir)/libzmq

bin_PROGRAMS = zmq_server
zmq_server_LDADD = $(top_builddir)/libzmq/libzmq.la
zmq_server_SOURCES = zmq_server.cpp 
zmq_server_CXXFLAGS = -Wall -pedantic -Wextra -Werror


