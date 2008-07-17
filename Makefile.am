if BUILD_PERF
DIR_PERF = perf
endif

SUBDIRS = zmq $(DIR_PERF) examples
DIST_SUBDIRS = zmq perf examples

EXTRA_DIST = \
    ./perf/helpers/ticker.hpp \
    ./perf/transports/openamq.hpp \
    ./perf/transports/sctp.hpp \
    ./perf/transports/tcp.hpp \
    ./perf/transports/ysuite.hpp \
    ./perf/transports/zmq.hpp \
    ./perf/tests/scenarios/thr.sh \
    ./perf/tests/scenarios/fo.sh \
    ./perf/tests/scenarios/fi.sh \
    ./examples/exchange/graph.pl

