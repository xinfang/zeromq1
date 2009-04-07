if BUILD_PERF
DIR_PERF = perf
endif

if BUILD_C
DIR_C = libczmq
endif

if BUILD_PYTHON
DIR_P = libpyzmq
endif

if BUILD_JAVA
DIR_J = libjzmq
endif

if BUILD_ZMQ_SERVER
DIR_ZMQ_SERVER = zmq_server
endif

if INSTALL_MAN
DIR_MAN = man
endif

if BUILD_CLRZMQ
DIR_CLR = mono/clrzmq/clrzmq
endif

SUBDIRS = libzmq $(DIR_C) $(DIR_P) $(DIR_J) $(DIR_PERF) $(DIR_ZMQ_SERVER) \
examples $(DIR_MAN) $(DIR_CLR)
DIST_SUBDIRS = libzmq libczmq libpyzmq libjzmq perf zmq_server examples man \
mono/clrzmq/clrzmq

EXTRA_DIST = \
    $(top_srcdir)/perf/tests/scenarios/thr.sh \
    $(top_srcdir)/perf/tests/scenarios/lat.sh \
    $(top_srcdir)/examples/exchange/graph.pl \
    $(top_srcdir)/perf/tests/zmq/py_*.py \
    $(top_srcdir)/perf/tests/zmq/f_*.for \
    $(top_srcdir)/perf/tests/zmq/cs_*.cs \
    $(top_srcdir)/man/man3/*.3 \
    $(top_srcdir)/openvms/*.com \
    $(top_srcdir)/openvms/platform.hpp \
    $(top_srcdir)/libvmszmq/vmszmq.c \
    $(top_srcdir)/mono/clrzmq/clrzmq/clrzmq.csproj \
    $(top_srcdir)/mono/clrzmq/cs_local_lat/cs_local_lat.csproj \
    $(top_srcdir)/mono/clrzmq/cs_remote_lat/cs_remote_lat.csproj \
    $(top_srcdir)/mono/clrzmq/cs_local_thr/cs_local_thr.csproj \
    $(top_srcdir)/mono/clrzmq/cs_remote_thr/cs_remote_thr.csproj

