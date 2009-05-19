INCLUDES = -I$(top_builddir) -I$(top_srcdir)/libzmq -I$(top_builddir)/libzmq

lib_LTLIBRARIES = librbzmq.la

librbzmq_la_SOURCES = zmq.cpp

librbzmq_la_LDFLAGS = -version-info 0:0:0
librbzmq_la_CXXFLAGS = -Wall -pedantic -Werror -Wno-long-long
librbzmq_la_LIBADD = $(top_builddir)/libzmq/libzmq.la

