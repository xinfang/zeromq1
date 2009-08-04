INCLUDES = -I$(top_builddir) -I$(top_srcdir)  -I$(top_builddir)/libzmq \
-I$(top_srcdir)/libzmq -I$(top_builddir)/libczmq  -I$(top_srcdir)/libczmq

lib_LTLIBRARIES = libluazmq.la

libluazmq_la_SOURCES = zmq.c

libluazmq_la_LDFLAGS = -version-info @LUALTVER@ @LUA_LIBS@
libluazmq_la_CFLAGS = -Wall -pedantic -Wno-long-long @LUA_CFLAGS@
libluazmq_la_LIBADD = $(top_builddir)/libczmq/libczmq.la
