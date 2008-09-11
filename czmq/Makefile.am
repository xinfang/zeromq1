INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS = czmq.h

lib_LTLIBRARIES = libczmq.la

libczmq_la_SOURCES = $(pkginclude_HEADERS) czmq.cpp

libczmq_la_LDFLAGS = -version-info 0:0:0
libczmq_la_CXXFLAGS = -Wall -pedantic
libczmq_la_LIBADD = $(top_builddir)/zmq/libzmq.la
