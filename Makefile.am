if BUILD_PERF
DIR_PERF = perf
endif

if BUILD_C
DIR_C = czmq
endif

if BUILD_PYTHON
DIR_P = pyzmq
endif

if BUILD_JAVA
DIR_J = jzmq
endif

SUBDIRS = zmq $(DIR_C) $(DIR_P) $(DIR_J) $(DIR_PERF) examples man
DIST_SUBDIRS = zmq czmq pyzmq jzmq perf examples man

EXTRA_DIST = \
    ./perf/tests/scenarios/thr.sh \
    ./perf/tests/scenarios/lat.sh \
    ./perf/tests/scenarios/fo.sh \
    ./perf/tests/scenarios/fi.sh \
    ./examples/exchange/graph.pl \
	./perf/tests/zmq/py_local_lat.py \
	./perf/tests/zmq/py_local_thr.py \
	./perf/tests/zmq/py_remote_lat.py \
	./perf/tests/zmq/py_remote_thr.py

