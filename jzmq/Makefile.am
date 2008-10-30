# We do not want to install Jzmq.class file
# user has to copy it to the right location.
#jzmqdir = /tmp

dist_noinst_JAVA = Jzmq.java

lib_LTLIBRARIES = libjzmq.la
libjzmq_la_SOURCES = Jzmq.h Jzmq.cpp
libjzmq_la_CXXFLAGS = @JAVA_INCLUDE@ -Wall
libjzmq_la_LDFLAGS = -version-info 0:0:0
libjzmq_la_LIBADD = $(top_builddir)/zmq/libzmq.la

BUILT_SOURCES = Jzmq.h Jzmq.class

$(srcdir)/Jzmq.cpp: Jzmq.h

Jzmq.h: Jzmq.class
	$(CLASSPATH_ENV) $(JAVAH) -jni -classpath ./ Jzmq

Jzmq.class: classdist_noinst.stamp

dist-hook:
		-rm $(distdir)/Jzmq.h
