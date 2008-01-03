#MAINTAINERCLEANFILES    = Makefile.in

INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS      = atomic_ptr.h atomic_uint32.h err.h kernel.h \
						tcp_socket.h wire.h ypipe.h ypollset.h ysemaphore.h \
						ysocketpair.h

lib_LIBRARIES = libzmq.a
libzmq_a_SOURCES = tcp_socket.cpp kernel.cpp
