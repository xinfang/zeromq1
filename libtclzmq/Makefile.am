INCLUDES = -I$(top_builddir) -I$(top_srcdir)  -I$(top_builddir)/libzmq \
-I$(top_srcdir)/libzmq -I$(top_builddir)/libczmq  -I$(top_srcdir)/libczmq

lib_LTLIBRARIES = libtclzmq.la

libtclzmq_la_SOURCES = zmq.c

libtclzmq_la_LDFLAGS = -version-info @TCLLTVER@
libtclzmq_la_CFLAGS = -Wall -pedantic -Wno-long-long -I@TCL_DIR@
libtclzmq_la_LIBADD = $(top_builddir)/libczmq/libczmq.la
