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
DIR_J = jzmq
endif

if BUILD_ZMQ_SERVER
DIR_ZMQ_SERVER = zmq_server
endif

SUBDIRS = libzmq $(DIR_C) $(DIR_P) $(DIR_J) $(DIR_PERF) $(DIR_ZMQ_SERVER) examples man
DIST_SUBDIRS = libzmq libczmq libpyzmq jzmq perf zmq_server examples man

EXTRA_DIST = \
    ./perf/tests/scenarios/thr.sh \
    ./perf/tests/scenarios/lat.sh \
    ./perf/tests/scenarios/fo.sh \
    ./perf/tests/scenarios/fi.sh \
    ./examples/exchange/graph.pl \
    ./perf/tests/zmq/py_local_lat.py \
    ./perf/tests/zmq/py_local_thr.py \
    ./perf/tests/zmq/py_remote_lat.py \
    ./perf/tests/zmq/py_remote_thr.py \
    ./man/man3/*.3
