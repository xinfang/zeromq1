# We do not want to install Jzmq.class file
# user has to copy it to the right location.
#jzmqdir = /tmp

dist_noinst_JAVA = Zmq.java

lib_LTLIBRARIES = libjzmq.la
libjzmq_la_SOURCES = Zmq.h Zmq.cpp
libjzmq_la_CXXFLAGS = -I$(top_builddir)/libzmq -I$(top_srcdir)/libzmq \
@JAVA_INCLUDE@ -I$(top_builddir)/libjzmq -I$(top_srcdir)/libjzmq -Wall
libjzmq_la_LDFLAGS = -version-info 0:0:0
libjzmq_la_LIBADD = $(top_builddir)/libzmq/libzmq.la

BUILT_SOURCES = Zmq.h Zmq.class

$(srcdir)/Zmq.cpp: Zmq.h

Zmq.h: Zmq.class
	$(CLASSPATH_ENV) $(JAVAH) -jni -classpath ./ Zmq

Zmq.class: classdist_noinst.stamp

dist-hook:
		-rm $(distdir)/Zmq.h
