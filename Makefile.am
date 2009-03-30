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

SUBDIRS = libzmq $(DIR_C) $(DIR_P) $(DIR_J) $(DIR_PERF) $(DIR_ZMQ_SERVER) examples $(DIR_MAN)
DIST_SUBDIRS = libzmq libczmq libpyzmq libjzmq perf zmq_server examples man

EXTRA_DIST = \
    $(top_srcdir)/perf/tests/scenarios/thr.sh \
    $(top_srcdir)/perf/tests/scenarios/lat.sh \
    $(top_srcdir)/examples/exchange/graph.pl \
    $(top_srcdir)/perf/tests/zmq/py_local_lat.py \
    $(top_srcdir)/perf/tests/zmq/py_local_thr.py \
    $(top_srcdir)/perf/tests/zmq/py_remote_lat.py \
    $(top_srcdir)/perf/tests/zmq/py_remote_thr.py \
    $(top_srcdir)/man/man3/*.3 \
    $(top_srcdir)/openvms/*.com \
    $(top_srcdir)/openvms/platform.hpp
