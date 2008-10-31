INCLUDES                = -I$(top_builddir) -I$(top_srcdir) $(PYTHON_INCLUDES)

pyexec_LTLIBRARIES = libpyzmq.la
libpyzmq_la_SOURCES = pyzmq.cpp
libpyzmq_la_LIBADD = $(top_builddir)/zmq/libzmq.la
