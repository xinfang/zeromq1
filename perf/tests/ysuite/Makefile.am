INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

noinst_PROGRAMS = ypipe_thr ypipe_lat

ypipe_thr_SOURCES = ypipe_thr.cpp test.hpp ../../transports/ysuite.hpp \
../../helpers/time.hpp ../../helpers/files.hpp ../../workers/raw_receiver.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp ../../workers/raw_sender.hpp

ypipe_lat_SOURCES = ypipe_lat.cpp test.hpp ../../transports/ysuite.hpp \
../../helpers/time.hpp ../../helpers/files.hpp ../../workers/echo.hpp \
../../interfaces/i_transport.hpp ../../interfaces/i_worker.hpp ../../workers/raw_ping_pong.hpp

ypipe_thr_LDADD = $(top_builddir)/zmq/libzmq.la
ypipe_lat_LDADD = $(top_builddir)/zmq/libzmq.la
