INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

EXTRA_DIST = common.hpp

noinst_PROGRAMS = gtw me stat

gtw_SOURCES = gtw.cpp ../../zmq/dispatcher.hpp \
../../zmq/api_engine.hpp ../../zmq/poll_thread.hpp \
../../zmq/msg.hpp ticker.hpp messages.hpp frequency_meter.hpp

me_SOURCES = me.cpp ../../zmq/dispatcher.hpp \
../../zmq/api_engine.hpp ../../zmq/poll_thread.hpp \
../../zmq/msg.hpp messages.hpp time.hpp frequency_meter.hpp \
matching_engine.hpp

stat_SOURCES = stat.cpp ../../zmq/dispatcher.hpp \
../../zmq/api_engine.hpp ../../zmq/poll_thread.hpp \
../../zmq/msg.hpp messages.hpp 

gtw_LDADD = $(top_builddir)/zmq/libzmq.la

me_LDADD = $(top_builddir)/zmq/libzmq.la

stat_LDADD = $(top_builddir)/zmq/libzmq.la
