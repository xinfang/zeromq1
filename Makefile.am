if BUILD_PERF
DIR_PERF = perf
endif

if BUILD_C
DIR_C = czmq
endif

SUBDIRS = zmq $(DIR_C) $(DIR_PERF) examples
DIST_SUBDIRS = zmq czmq perf examples

EXTRA_DIST = \
    ./perf/tests/scenarios/thr.sh \
    ./perf/tests/scenarios/lat.sh \
    ./perf/tests/scenarios/fo.sh \
    ./perf/tests/scenarios/fi.sh \
    ./examples/exchange/graph.pl

