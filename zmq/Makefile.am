INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS      = kernel.h

lib_LTLIBRARIES = libzmq.la
libzmq_la_SOURCES = tcp_socket.cpp kernel.cpp atomic_ptr.h atomic_uint32.h err.h \
tcp_socket.h wire.h ypipe.h ypollset.h ysemaphore.h ysocketpair.h

libzmq_la_LDFLAGS = -version-info 0:0:0
