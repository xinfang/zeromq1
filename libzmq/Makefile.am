INCLUDES                = -I$(top_builddir) -I$(top_srcdir)

include_HEADERS = zmq.hpp

pkginclude_HEADERS = \
    ./zmq/api_thread.hpp \
    ./zmq/atomic_ptr.hpp \
    ./zmq/atomic_bitmap.hpp \
    ./zmq/atomic_counter.hpp \
    ./zmq/bp_decoder.hpp \
    ./zmq/decoder.hpp \
    ./zmq/dispatcher.hpp \
    ./zmq/err.hpp \
    ./zmq/i_signaler.hpp \
    ./zmq/tcp_socket.hpp \
    ./zmq/wire.hpp \
    ./zmq/ypipe.hpp \
    ./zmq/ypollset.hpp \
    ./zmq/ysemaphore.hpp \
    ./zmq/ysocketpair.hpp \
    ./zmq/io_thread.hpp \
    ./zmq/poller.hpp \
    ./zmq/poll_thread.hpp \
    ./zmq/epoll_thread.hpp \
    ./zmq/devpoll_thread.hpp \
    ./zmq/kqueue_thread.hpp \
    ./zmq/bp_encoder.hpp \
    ./zmq/encoder.hpp \
    ./zmq/stdint.hpp \
    ./zmq/bp_tcp_engine.hpp \
    ./zmq/i_pollable.hpp \
    ./zmq/command.hpp \
    ./zmq/mux.hpp \
    ./zmq/i_demux.hpp \
    ./zmq/publisher.hpp \
    ./zmq/load_balancer.hpp \
    ./zmq/i_thread.hpp \
    ./zmq/pipe.hpp \
    ./zmq/bp_tcp_listener.hpp \
    ./zmq/locator.hpp \
    ./zmq/i_engine.hpp \
    ./zmq/scope.hpp \
    ./zmq/config.hpp \
    ./zmq/yqueue.hpp \
    ./zmq/tcp_listener.hpp \
    ./zmq/i_locator.hpp \
    ./zmq/raw_message.hpp \
    ./zmq/message.hpp \
    ./zmq/ip.hpp \
    ./zmq/i_poller.hpp \
    ./zmq/thread.hpp \
    ./zmq/mutex.hpp \
    ./zmq/platform.hpp \
    ./zmq/formatting.hpp \
    ./zmq/select_thread.hpp \
    ./zmq/export.hpp \
    ./zmq/server_protocol.hpp \
    ./zmq/windows.hpp \
    ./zmq/fd.hpp \
    ./zmq/out_engine.hpp \
    ./zmq/in_engine.hpp \
    ./zmq/engine_factory.hpp \
    ./zmq/sctp_listener.hpp \
    ./zmq/sctp_engine.hpp \
    ./zmq/pgm_socket.hpp \
    ./zmq/bp_pgm_sender.hpp \
    ./zmq/bp_pgm_receiver.hpp \
    ./zmq/amqp_client.hpp \
    ./zmq/amqp_encoder.hpp \
    ./zmq/amqp_decoder.hpp \
    ./zmq/amqp_marshaller.hpp \
    ./zmq/amqp_unmarshaller.hpp \
    ./zmq/i_amqp.hpp \
    ./zmq/engine_base.hpp \
    ./zmq/xmlParser.hpp \
    ./zmq/data_dam.hpp

lib_LTLIBRARIES = libzmq.la

if BUILD_PGM
pgm_sources = ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/packet.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/timer.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/if.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/gsi.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/signal.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/txwi.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/rxwi.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/transport.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/rate_control.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/async.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/checksum.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/reed_solomon.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/version.c \
    ../third-party/openpgm/libpgm-1.0.0/openpgm/pgm/galois_tables.c
endif

libzmq_la_SOURCES = $(pkginclude_HEADERS) $(pgm_sources) \
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
    bp_tcp_engine.cpp \
    mux.cpp \
    publisher.cpp \
    load_balancer.cpp \
    pipe.cpp \
    bp_tcp_listener.cpp \
    locator.cpp \
    tcp_listener.cpp \
    ip.cpp \
    thread.cpp \
    select_thread.cpp \
    out_engine.cpp \
    in_engine.cpp \
    engine_factory.cpp \
    sctp_listener.cpp \
    sctp_engine.cpp \
    pgm_socket.cpp \
    bp_pgm_sender.cpp \
    bp_pgm_receiver.cpp \
    amqp_client.cpp \
    amqp_encoder.cpp \
    amqp_decoder.cpp \
    amqp_marshaller.cpp \
    amqp_unmarshaller.cpp \
    xmlParser.cpp \
    data_dam.cpp


libzmq_la_LDFLAGS = -version-info @LTVER@

if BUILD_PGM
libzmq_la_CXXFLAGS = -I$(top_srcdir)/third-party/openpgm/libpgm-1.0.0/openpgm/pgm/include/ -Wall -Werror @LIBZMQ_EXTRA_CXXFLAGS@
libzmq_la_CFLAGS = -I$(top_srcdir)/third-party/openpgm/libpgm-1.0.0/openpgm/pgm/include/ \
    -pipe \
    -Wall \
    -Wextra \
    -Wfloat-equal \
    -Wshadow \
    -Wunsafe-loop-optimizations \
    -Wpointer-arith \
    -Wbad-function-cast \
    -Wcast-qual \
    -Wcast-align \
    -Wwrite-strings \
    -Waggregate-return \
    -Wstrict-prototypes \
    -Wold-style-definition \
    -Wmissing-prototypes \
    -Wmissing-declarations \
    -Wmissing-noreturn \
    -Wmissing-format-attribute \
    -Wredundant-decls \
    -Wnested-externs \
    -Winline \
    -pedantic \
    -std=gnu99 \
    --param max-inline-insns-single=600 \
    -D_REENTRANT \
    -D_GNU_SOURCE \
    -D__need_IOV_MAX \
    -DCONFIG_HAVE_EPOLL \
    -DCONFIG_HAVE_RTC \
    -DCONFIG_HAVE_TSC \
    -DCONFIG_HAVE_IFR_NETMASK \
    -DCONFIG_HAVE_GETIFADDRS \
    -DCONFIG_HAVE_GETHOSTBYNAME2 \
    -DCONFIG_HAVE_GETPROTOBYNAME_R \
    -DCONFIG_HAVE_SIGHANDLER_T \
    -DCONFIG_BIND_INADDR_ANY \
    -DCONFIG_GALOIS_MUL_LUT
else
libzmq_la_CXXFLAGS = -Wall -Werror @LIBZMQ_EXTRA_CXXFLAGS@
endif

dist-hook:
		-rm $(distdir)/zmq/platform.hpp


