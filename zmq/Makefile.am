INCLUDES                = -I$(top_builddir) -I$(top_srcdir)
INCLUDES += -I/home/malosek/openPGM/pgm/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

LIBS=-lglib-2.0 -lgthread-2.0 /home/malosek/openPGM/pgm/ref/debug/libpgm.a

pkginclude_HEADERS = \
    api_engine.hpp \
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
    poll_thread.hpp \
    bp_encoder.hpp \
    encoder.hpp \
    platform.hpp \
    stdint.hpp \
    free_fn.hpp \
    i_pollable.hpp \
    amqp09_encoder.hpp \
    amqp09_decoder.hpp \
    i_amqp09.hpp \
    amqp09_marshaller.hpp \
    amqp09_unmarshaller.hpp \
	pgm_socket.hpp \
	epgm_socket.hpp \
	pgm_sender_engine.hpp \
	pgm_receiver_engine.hpp

#	bp_engine.hpp
#	amqp09_engine.hpp
#	amqp09_client_fsm.hpp
#	amqp09_server_fsm.hpp

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
    api_engine.hpp \
    api_engine.cpp \
    poll_thread.hpp \
    poll_thread.cpp \
    platform.hpp \
    stdint.hpp \
    free_fn.hpp \
    i_pollable.hpp \
    amqp09_encoder.hpp \
    amqp09_encoder.cpp \
    amqp09_decoder.hpp \
    amqp09_decoder.cpp \
    i_amqp09.hpp \
    amqp09_marshaller.hpp \
    amqp09_marshaller.cpp \
    amqp09_unmarshaller.hpp \
    amqp09_unmarshaller.cpp \
	pgm_socket.cpp \
	pgm_socket.hpp \
	epgm_socket.hpp \
	epgm_socket.cpp \
	pgm_sender_engine.hpp \
	pgm_sender_engine.cpp \
	pgm_receiver_engine.hpp \
	pgm_receiver_engine.cpp

#    bp_engine.hpp
#    bp_engine.cpp
#    amqp09_engine.hpp
#    amqp09_client_fsm.hpp
#    amqp09_client_fsm.cpp
#    amqp09_server_fsm.hpp
#    amqp09_server_fsm.cpp

libzmq_la_LDFLAGS = -version-info 0:0:0

dist-hook:
		-rm $(distdir)/stdint.hpp
		-rm $(distdir)/platform.hpp


