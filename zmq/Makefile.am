INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS = \
    api_thread.hpp \
    atomic_ptr.hpp \
    atomic_uint32.hpp \
    bp_decoder.hpp \
    cmsg.hpp \
    decoder.hpp \
    dispatcher.hpp \
    dispatcher_proxy.hpp \
    err.hpp \
    i_signaler.hpp \
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
    free_fn.hpp \
    bp_engine.hpp \
    i_engine.hpp \
    amqp09_encoder.hpp \
    amqp09_decoder.hpp \
    amqp09_engine.hpp \
    amqp09_client_fsm.hpp \
    i_amqp09.hpp \
    amqp09_constants.hpp \
    amqp09_marshaller.hpp \
    amqp09_unmarshaller.hpp

lib_LTLIBRARIES = libzmq.la
libzmq_la_SOURCES = \
    tcp_socket.hpp \
    tcp_socket.cpp \
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
    encoder.hpp \
    bp_encoder.hpp \
    bp_encoder.cpp \
    api_thread.hpp \
    api_thread.cpp \
    io_thread.hpp \
    io_thread.cpp \
    platform.hpp \
    stdint.hpp \
    free_fn.hpp \
    bp_engine.hpp \
    bp_engine.cpp \
    i_engine.hpp \
    amqp09_encoder.hpp \
    amqp09_encoder.cpp \
    amqp09_decoder.hpp \
    amqp09_decoder.cpp \
    amqp09_engine.hpp \
    amqp09_client_fsm.hpp \
    amqp09_client_fsm.cpp \
    i_amqp09.hpp \
    amqp09_constants.hpp \
    amqp09_marshaller.hpp \
    amqp09_marshaller.cpp \
    amqp09_unmarshaller.hpp \
    amqp09_unmarshaller.cpp

libzmq_la_LDFLAGS = -version-info 0:0:0

dist-hook:
		-rm $(distdir)/stdint.hpp
		-rm $(distdir)/platform.hpp


