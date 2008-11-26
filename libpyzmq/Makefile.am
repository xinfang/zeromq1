INCLUDES = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/libzmq \
-I$(top_builddir)/libzmq $(PYTHON_INCLUDES)

pyexec_LTLIBRARIES = libpyzmq.la
libpyzmq_la_SOURCES = pyzmq.cpp
libpyzmq_la_LIBADD = $(top_builddir)/libzmq/libzmq.la
