INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq


noinst_PROGRAMS = receiver sender display

receiver_SOURCES = receiver.cpp
receiver_LDADD = $(top_builddir)/zmq/libzmq.la
receiver_LDFLAGS = @SDL_LIBS@
receiver_CXXFLAGS = -Wall @SDL_CFLAGS@

sender_SOURCES = sender.cpp
sender_LDADD = $(top_builddir)/zmq/libzmq.la
sender_LDFLAGS = @UNICAP_LIBS@ @UCIL_LIBS@
sender_CXXFLAGS = -Wall @UNICAP_CFLAGS@ @UCIL_CFLAGS@

display_SOURCES = display.c
display_LDADD = $(top_builddir)/zmq/libzmq.la
display_LDFLAGS = @UNICAP_LIBS@ @UCIL_LIBS@ @SDL_LIBS@ 
display_CFLAGS = -Wall @UNICAP_CFLAGS@ @UCIL_CFLAGS@ @SDL_CFLAGS@
