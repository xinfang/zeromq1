INCLUDES        = -I$(top_builddir) -I$(top_builddir)/libzmq \
                -I$(top_srcdir) -I$(top_srcdir)/libzmq

noinst_PROGRAMS = gtw me stat

gtw_SOURCES = gtw.cpp ../../libzmq/zmq/dispatcher.hpp ../../libzmq/zmq/api_thread.hpp \
../../libzmq/zmq/poll.hpp messages.hpp frequency_meter.hpp common.hpp
gtw_LDADD = $(top_builddir)/libzmq/libzmq.la
gtw_CXXFLAGS = -Wall -pedantic -Werror

me_SOURCES = me.cpp ../../libzmq/zmq/dispatcher.hpp ../../libzmq/zmq/api_thread.hpp \
../../libzmq/zmq/poll.hpp messages.hpp frequency_meter.hpp \
matching_engine.hpp common.hpp
me_LDADD = $(top_builddir)/libzmq/libzmq.la
me_CXXFLAGS = -Wall -pedantic -Werror

stat_SOURCES = stat.cpp ../../libzmq/zmq/dispatcher.hpp ../../libzmq/zmq/api_thread.hpp \
../../libzmq/zmq/poll.hpp messages.hpp common.hpp 
stat_LDADD = $(top_builddir)/libzmq/libzmq.la
stat_CXXFLAGS = -Wall -pedantic -Werror
