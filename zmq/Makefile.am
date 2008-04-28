INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS = \
    api_engine.hpp \
    atomic_ptr.hpp \
    atomic_bitmap.hpp \
    atomic_counter.hpp \
    bp_decoder.hpp \
    decoder.hpp \
    dispatcher.hpp \
    err.hpp \
    i_signaler.hpp \
    tcp_socket.hpp \
    wire.hpp \
    ypipe.hpp \
    ypollset.hpp \
    ysemaphore.hpp \
    ysocketpair.hpp \
    poll_thread.hpp \
    bp_encoder.hpp \
    encoder.hpp \
    platform.hpp \
    stdint.hpp \
    free_fn.hpp \
    bp_engine.hpp \
    i_pollable.hpp \
    amqp09_encoder.hpp \
    amqp09_decoder.hpp \
    amqp09_engine.hpp \
    amqp09_client_fsm.hpp \
    amqp09_server_fsm.hpp \
    i_amqp09.hpp \
    amqp09_marshaller.hpp \
    amqp09_unmarshaller.hpp \
    command.hpp \
    mux.hpp \
    demux.hpp \
    msg.hpp \
    i_thread.hpp \
    pipe.hpp \
    bp_listener.hpp \
    amqp09_listener.hpp \
    locator.hpp \
    amqp09_client_engine.hpp \
    amqp09_server_engine.hpp

lib_LTLIBRARIES = libzmq.la
libzmq_la_SOURCES = \
    tcp_socket.hpp \
    tcp_socket.cpp \
    ysocketpair.hpp \
    ysocketpair.cpp \
    atomic_ptr.hpp \
    atomic_bitmap.hpp \
    atomic_counter.hpp \
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
    dispatcher.hpp \
    dispatcher.cpp \
    decoder.hpp \
    bp_decoder.hpp \
    bp_decoder.cpp \
    encoder.hpp \
    bp_encoder.hpp \
    bp_encoder.cpp \
    api_engine.hpp \
    api_engine.cpp \
    poll_thread.hpp \
    poll_thread.cpp \
    platform.hpp \
    stdint.hpp \
    free_fn.hpp \
    bp_engine.hpp \
    bp_engine.cpp \
    i_pollable.hpp \
    amqp09_encoder.hpp \
    amqp09_encoder.cpp \
    amqp09_decoder.hpp \
    amqp09_decoder.cpp \
    amqp09_engine.hpp \
    amqp09_client_fsm.hpp \
    amqp09_client_fsm.cpp \
    amqp09_server_fsm.hpp \
    amqp09_server_fsm.cpp \
    i_amqp09.hpp \
    amqp09_marshaller.hpp \
    amqp09_marshaller.cpp \
    amqp09_unmarshaller.hpp \
    amqp09_unmarshaller.cpp \
    command.hpp \
    mux.hpp \
    mux.cpp \
    demux.hpp \
    demux.cpp \
    msg.hpp \
    i_thread.hpp \
    pipe.hpp \
    pipe.cpp \
    bp_listener.hpp \
    bp_listener.cpp \
    amqp09_listener.hpp \
    amqp09_listener.cpp \
    locator.hpp \
    locator.cpp \
    amqp09_client_engine.hpp \
    amqp09_server_engine.hpp

libzmq_la_LDFLAGS = -version-info 0:0:0

dist-hook:
		-rm $(distdir)/stdint.hpp
		-rm $(distdir)/platform.hpp


