# We do not want to install Jzmq.class file
# user has to copy it to the right location.
#jzmqdir = /tmp

dist_noinst_JAVA = Jzmq.java

lib_LTLIBRARIES = libjzmq.la
libjzmq_la_SOURCES = ./zmq/Jzmq.h Jzmq.cpp
libjzmq_la_CXXFLAGS = -I$(top_builddir)/libzmq -I$(top_srcdir)/libzmq \
@JAVA_INCLUDE@ -I$(top_builddir)/libjzmq -I$(top_srcdir)/libjzmq -Wall
libjzmq_la_LDFLAGS = -version-info 0:0:0
libjzmq_la_LIBADD = $(top_builddir)/libzmq/libzmq.la

BUILT_SOURCES = ./zmq/Jzmq.h Jzmq.class

$(srcdir)/Jzmq.cpp: ./zmq/Jzmq.h

./zmq/Jzmq.h: Jzmq.class
	$(CLASSPATH_ENV) $(JAVAH) -jni -d ./zmq -classpath ./ Jzmq

Jzmq.class: classdist_noinst.stamp

dist-hook:
		-rm $(distdir)/zmq/Jzmq.h
