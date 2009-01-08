INCLUDES        = -I$(top_builddir) -I$(top_builddir)/libzmq \
                -I$(top_srcdir) -I$(top_srcdir)/libzmq

if BUILD_C
C_TEST_BINS = c_local_lat c_remote_lat c_local_thr c_remote_thr
endif

if BUILD_OPENPGM
PGM_TEST_BINS = pgm_sender pgm_receiver
endif

noinst_PROGRAMS = local_lat remote_lat local_thr remote_thr \
local_fo remote_fo local_fi remote_fi local_thr_lat remote_thr_lat \
$(C_TEST_BINS) $(PGM_TEST_BINS)

local_thr_SOURCES = local_thr.cpp ../../transports/zmq_transport.hpp \
../../transports/i_transport.hpp ../../helpers/functions.hpp\
../scenarios/thr.hpp
local_thr_LDADD = $(top_builddir)/libzmq/libzmq.la 
local_thr_CXXFLAGS = -Wall -pedantic -Werror

remote_thr_SOURCES = remote_thr.cpp ../../transports/zmq_transport.hpp \
../scenarios/thr.hpp ../../transports/i_transport.hpp 
remote_thr_LDADD = $(top_builddir)/libzmq/libzmq.la
remote_thr_CXXFLAGS = -Wall -pedantic -Werror

local_lat_SOURCES = local_lat.cpp ../../transports/zmq_transport.hpp \
../../transports/i_transport.hpp \
../scenarios/lat.hpp
local_lat_LDADD = $(top_builddir)/libzmq/libzmq.la
local_lat_CXXFLAGS = -Wall -pedantic -Werror

remote_lat_SOURCES = remote_lat.cpp ../../transports/zmq_transport.hpp \
../../transports/i_transport.hpp ../scenarios/lat.hpp 
remote_lat_LDADD = $(top_builddir)/libzmq/libzmq.la
remote_lat_CXXFLAGS = -Wall -pedantic -Werror

local_fo_SOURCES = local_fo.cpp ../../transports/zmq_transport.hpp \
../../transports/i_transport.hpp ../scenarios/fo.hpp
local_fo_LDADD = $(top_builddir)/libzmq/libzmq.la
local_fo_CXXFLAGS = -Wall -pedantic -Werror

remote_fo_SOURCES = remote_fo.cpp ../../transports/zmq_transport.hpp \
../../transports/i_transport.hpp ../scenarios/fo.hpp
remote_fo_LDADD = $(top_builddir)/libzmq/libzmq.la
remote_fo_CXXFLAGS = -Wall -pedantic -Werror

local_fi_SOURCES = local_fi.cpp ../../transports/zmq_transport.hpp \
../../transports/i_transport.hpp ../scenarios/fi.hpp
local_fi_LDADD = $(top_builddir)/libzmq/libzmq.la
local_fi_CXXFLAGS = -Wall -pedantic -Werror

remote_fi_SOURCES = remote_fi.cpp ../../transports/zmq_transport.hpp \
../../transports/i_transport.hpp ../scenarios/fi.hpp
remote_fi_LDADD = $(top_builddir)/libzmq/libzmq.la
remote_fi_CXXFLAGS = -Wall -pedantic -Werror

local_thr_lat_SOURCES = local_thr_lat.cpp ../../transports/zmq_transport.hpp \
../../helpers/time.hpp ../../helpers/functions.hpp \
../../transports/i_transport.hpp ../scenarios/thr_lat.hpp \
../../helpers/ticker.hpp
local_thr_lat_LDADD = $(top_builddir)/libzmq/libzmq.la
local_thr_lat_CXXFLAGS = -Wall -pedantic -Werror

remote_thr_lat_SOURCES = remote_thr_lat.cpp ../../transports/zmq_transport.hpp \
../../helpers/time.hpp ../../helpers/functions.hpp \
../../transports/i_transport.hpp ../scenarios/thr_lat.hpp \
../../helpers/ticker.hpp
remote_thr_lat_LDADD = $(top_builddir)/libzmq/libzmq.la
remote_thr_lat_CXXFLAGS = -Wall -pedantic -Werror

if BUILD_C
c_local_lat_SOURCES = c_local_lat.c ../../../libczmq/zmq/czmq.h
c_local_lat_LDADD = $(top_builddir)/libczmq/libczmq.la \
$(top_builddir)/libzmq/libzmq.la -lstdc++
c_local_lat_CFLAGS = -Wall -pedantic -Werror -I$(top_srcdir)/libczmq

c_remote_lat_SOURCES = c_remote_lat.c ../../../libczmq/zmq/czmq.h
c_remote_lat_LDADD = $(top_builddir)/libczmq/libczmq.la \
$(top_builddir)/libzmq/libzmq.la -lstdc++
c_remote_lat_CFLAGS = -Wall -pedantic -Werror -I$(top_srcdir)/libczmq

c_local_thr_SOURCES = c_local_thr.c ../../../libczmq/zmq/czmq.h
c_local_thr_LDADD = $(top_builddir)/libczmq/libczmq.la \
$(top_builddir)/libzmq/libzmq.la -lstdc++
c_local_thr_CFLAGS = -Wall -pedantic -Werror -I$(top_srcdir)/libczmq

c_remote_thr_SOURCES = c_remote_thr.c ../../../libczmq/zmq/czmq.h
c_remote_thr_LDADD = $(top_builddir)/libczmq/libczmq.la \
$(top_builddir)/libzmq/libzmq.la -lstdc++
c_remote_thr_CFLAGS = -Wall -pedantic -Werror -I$(top_srcdir)/libczmq
endif

if BUILD_JAVA
AM_JAVACFLAGS=-classpath $(top_builddir)/libjzmq 

dist_noinst_JAVA = j_local_lat.java j_remote_lat.java j_local_thr.java \
j_remote_thr.java
endif

if BUILD_OPENPGM
pgm_sender_SOURCES = pgm_sender.cpp
pgm_sender_LDADD = $(top_builddir)/libzmq/libzmq.la
pgm_sender_CPPFLAGS = -Wall -pedantic -Werror -I$(top_srcdir)/libczmq

pgm_receiver_SOURCES = pgm_receiver.cpp
pgm_receiver_LDADD = $(top_builddir)/libzmq/libzmq.la
pgm_receiver_CPPFLAGS = -Wall -pedantic -Werror -I$(top_srcdir)/libczmq
endif
