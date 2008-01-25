INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS = \
    kernel.hpp \
    api_thread.hpp \
    atomic_ptr.hpp \
    atomic_uint32.hpp \
    bp_decoder.hpp \
	amqp_decoder.hpp \
    cmsg.hpp \
    decoder.hpp \
    dispatcher.hpp \
    dispatcher_proxy.hpp \
    err.hpp \
    i_signaler.hpp \
    i_socket.hpp \
    tcp_socket.hpp \
    wire.hpp \
    ydispatcher.hpp \
    ydispatcher_proxy.hpp \
    ypipe.hpp \
    ypollset.hpp \
    ysemaphore.hpp \
    ysocketpair.hpp \
    io_thread.hpp \
    bp_encoder.hpp \
    encoder.hpp \
    platform.hpp \
    stdint.hpp \
    amqp09_base.hpp \
    amqp09_server_socket.hpp

lib_LTLIBRARIES = libzmq.la
libzmq_la_SOURCES = \
    tcp_socket.hpp \
    tcp_socket.cpp \
    kernel.hpp \
    kernel.cpp \
    ysocketpair.hpp \
    ysocketpair.cpp \
    atomic_ptr.hpp \
    atomic_uint32.hpp \
    err.hpp \
    wire.hpp \
    ypipe.hpp \
    ypollset.hpp \
    ypollset.cpp \
    ysemaphore.hpp \
    ysocketpair.hpp \
    i_signaler.hpp \
    i_socket.hpp \
    ysemaphore.hpp \
    ysemaphore.cpp \
    ydispatcher.hpp \
    dispatcher.hpp \
    ydispatcher_proxy.hpp \
    dispatcher_proxy.hpp \
    cmsg.hpp \
    decoder.hpp \
    bp_decoder.hpp \
    bp_decoder.cpp \
    amqp_decoder.hpp \
    amqp_decoder.cpp \
    encoder.hpp \
    bp_encoder.hpp \
    bp_encoder.cpp \
    api_thread.hpp \
    api_thread.cpp \
    io_thread.hpp \
    io_thread.cpp \
    platform.hpp \
    stdint.hpp \
    amqp09_base.hpp \
    amqp09_base.cpp \
    amqp09_server_socket.hpp \
    amqp09_server_socket.cpp

libzmq_la_LDFLAGS = -version-info 0:0:0

dist-hook:
		-rm $(distdir)/stdint.hpp
		-rm $(distdir)/platform.hpp


