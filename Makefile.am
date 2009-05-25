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

if BUILD_RUBY
DIR_R = librbzmq
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
examples $(DIR_MAN) $(DIR_CLR) $(DIR_R)
DIST_SUBDIRS = libzmq libczmq libpyzmq libjzmq perf zmq_server examples man \
mono/clrzmq/clrzmq librbzmq

EXTRA_DIST = \
    $(top_srcdir)/examples/exchange/graph.pl \
    $(top_srcdir)/perf/tests/zmq/py_*.py \
    $(top_srcdir)/perf/tests/zmq/f_*.for \
    $(top_srcdir)/perf/tests/zmq/cs_*.cs \
    $(top_srcdir)/perf/tests/zmq/cob_*.cob \
    $(top_srcdir)/perf/tests/zmq/rb_*.rb \
    $(top_srcdir)/man/man3/*.3 \
    $(top_srcdir)/openvms/*.com \
    $(top_srcdir)/openvms/platform.hpp \
    $(top_srcdir)/libvmszmq/vmszmq.c \
    $(top_srcdir)/libvmszmq/zmq.lib \
    $(top_srcdir)/libclrzmq/zmq.cs \
    $(top_srcdir)/librbzmq/extconf.rb \
    $(top_srcdir)/mono/clrzmq/clrzmq/zmq_strong_name.snk \
    $(top_srcdir)/mono/clrzmq/clrzmq/Properties/AssemblyInfo.cs

