INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

pkginclude_HEADERS = \
    api_thread.hpp \
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
    i_context.hpp \
    pipe.hpp \
    bp_listener.hpp \
    amqp09_listener.hpp \
    locator.hpp \
    amqp09_client_engine.hpp \
    amqp09_server_engine.hpp \
    i_engine.hpp \
    scope.hpp \
    global_locator.hpp \
    config.hpp \
    yqueue.hpp \
    tcp_listener.hpp \
    i_locator.hpp \
    raw_message.hpp \
    message.hpp

lib_LTLIBRARIES = libzmq.la

libzmq_la_SOURCES = $(pkginclude_HEADERS) \
    tcp_socket.cpp \
    ysocketpair.cpp \
    ypollset.cpp \
    ysemaphore.cpp \
    dispatcher.cpp \
    bp_decoder.cpp \
    bp_encoder.cpp \
    api_thread.cpp \
    poll_thread.cpp \
    bp_engine.cpp \
    amqp09_encoder.cpp \
    amqp09_decoder.cpp \
    amqp09_client_fsm.cpp \
    amqp09_server_fsm.cpp \
    amqp09_marshaller.cpp \
    amqp09_unmarshaller.cpp \
    mux.cpp \
    demux.cpp \
    pipe.cpp \
    bp_listener.cpp \
    amqp09_listener.cpp \
    locator.cpp \
    tcp_listener.cpp

libzmq_la_LDFLAGS = -version-info 0:0:0
libzmq_la_CXXFLAGS = -Wall -pedantic

bin_PROGRAMS = global_locator
global_locator_SOURCES = global_locator.cpp stdint.hpp err.hpp

dist-hook:
		-rm $(distdir)/stdint.hpp
		-rm $(distdir)/platform.hpp


