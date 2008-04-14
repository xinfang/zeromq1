INCLUDES        = -I$(top_builddir) -I$(top_builddir)/zmq \
                -I$(top_srcdir) -I$(top_srcdir)/zmq

check_PROGRAMS = test001
check_PROGRAMS += test002
check_PROGRAMS += test003
check_PROGRAMS += test004
check_PROGRAMS += test005
check_PROGRAMS += test006
check_PROGRAMS += test007
check_PROGRAMS += test008


TESTS = $(check_PROGRAMS)

#noinst_PROGRAMS = test001

test001_SOURCES = test001.cpp 
test002_SOURCES = test002.cpp 
test003_SOURCES = test003.cpp 
test004_SOURCES = test004.cpp 
test005_SOURCES = test005.cpp 
test006_SOURCES = test006.cpp 
test007_SOURCES = test007.cpp 
test008_SOURCES = test008.cpp 

test001_LDADD = $(top_builddir)/zmq/libzmq.la
test002_LDADD = $(top_builddir)/zmq/libzmq.la
test003_LDADD = $(top_builddir)/zmq/libzmq.la
test004_LDADD = $(top_builddir)/zmq/libzmq.la
test005_LDADD = $(top_builddir)/zmq/libzmq.la
test006_LDADD = $(top_builddir)/zmq/libzmq.la
test007_LDADD = $(top_builddir)/zmq/libzmq.la
test008_LDADD = $(top_builddir)/zmq/libzmq.la

