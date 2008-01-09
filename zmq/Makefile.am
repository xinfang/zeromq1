INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS      = kernel.hpp

lib_LTLIBRARIES = libzmq.la
libzmq_la_SOURCES = \
    tcp_socket.cpp \
    kernel.cpp \
    ysocketpair.cpp \
    atomic_ptr.hpp \
    atomic_uint32.hpp \
    err.hpp \
    tcp_socket.hpp \
    wire.hpp \
    ypipe.hpp \
    ypollset.hpp \
    ypollset.cpp \
    ysemaphore.hpp \
    ysocketpair.hpp \
    i_signaler.hpp \
    ysemaphore.cpp \
    ydispatcher.hpp \
    dispatcher.hpp \
    api_thread.hpp \
    api_thread.cpp \
    cmsg.hpp 
libzmq_la_LDFLAGS = -version-info 0:0:0
