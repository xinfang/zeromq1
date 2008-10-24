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
    zmq_server.hpp \
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
    pgm_socket.hpp \
    epgm_socket.hpp \
    pgm_receiver_engine.hpp \
    pgm_sender_engine.hpp

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
    bp_engine.cpp \
    mux.cpp \
    demux.cpp \
    pipe.cpp \
    bp_listener.cpp \
    locator.cpp \
    tcp_listener.cpp \
    ip.cpp \
    thread.cpp

if BUILD_OPENPGM
libzmq_la_SOURCES += pgm_socket.cpp epgm_socket.cpp \
pgm_receiver_engine.cpp pgm_sender_engine.cpp


libzmq_la_LDFLAGS = -version-info 0:0:0 @GLIB_LIBS@ @PGM_LIBS@
libzmq_la_CXXFLAGS = -Wall -pedantic @GLIB_CFLAGS@ @PGM_CFLAGS@
else
libzmq_la_LDFLAGS = -version-info 0:0:0
libzmq_la_CXXFLAGS = -Wall -pedantic
endif

bin_PROGRAMS = zmq_server
zmq_server_LDADD = libzmq.la
zmq_server_SOURCES = zmq_server.cpp stdint.hpp err.hpp
zmq_server_CXXFLAGS = -Wall -pedantic -Werror

dist-hook:
		-rm $(distdir)/platform.hpp


