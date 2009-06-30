INCLUDES = -I$(top_builddir) -I$(top_srcdir)  -I$(top_builddir)/libzmq \
-I$(top_srcdir)/libzmq -I$(top_builddir)/libczmq  -I$(top_srcdir)/libczmq

include_HEADERS = zmq.h

lib_LTLIBRARIES = libczmq.la

libczmq_la_SOURCES = $(pkginclude_HEADERS) zmq.cpp

libczmq_la_LDFLAGS = -version-info @CLTVER@
libczmq_la_CXXFLAGS = -Wall -pedantic
libczmq_la_LIBADD = $(top_builddir)/libzmq/libzmq.la
