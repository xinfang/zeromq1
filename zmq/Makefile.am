INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

include_HEADERS = zmq.hpp

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
    epoll_thread.hpp \
    devpoll_thread.hpp \
    kqueue_thread.hpp \
    bp_encoder.hpp \
    encoder.hpp \
    stdint.hpp \
    bp_engine.hpp \
    i_pollable.hpp \
    command.hpp \
    mux.hpp \
    demux.hpp \
    i_thread.hpp \
    pipe.hpp \
    bp_listener.hpp \
    locator.hpp \
    i_engine.hpp \
    scope.hpp \
    config.hpp \
    yqueue.hpp \
    tcp_listener.hpp \
    i_locator.hpp \
    raw_message.hpp \
    message.hpp \
    ip.hpp \
    i_poller.hpp \
    thread.hpp \
    mutex.hpp \
    platform.hpp \
    formatting.hpp \
    select_thread.hpp \
    export.hpp \
    server_protocol.hpp

lib_LTLIBRARIES = libzmq.la

libzmq_la_SOURCES = $(pkginclude_HEADERS) \
    tcp_socket.cpp \
    ysocketpair.cpp \
    ypollset.cpp \
    ysemaphore.cpp \
    dispatcher.cpp \
    err.cpp \
    bp_decoder.cpp \
    bp_encoder.cpp \
    api_thread.cpp \
    poll_thread.cpp \
    epoll_thread.cpp \
    devpoll_thread.cpp \
    kqueue_thread.cpp \
    bp_engine.cpp \
    mux.cpp \
    demux.cpp \
    pipe.cpp \
    bp_listener.cpp \
    locator.cpp \
    tcp_listener.cpp \
    ip.cpp \
    thread.cpp \
    select_thread.cpp

libzmq_la_LDFLAGS = -version-info 0:0:0
libzmq_la_CXXFLAGS = -Wall -pedantic

dist-hook:
		-rm $(distdir)/platform.hpp


