# We do not want to install Jzmq.class file
# user has to copy it to the right location.
#jzmqdir = /tmp

dist_noinst_JAVA = ./org/zmq/Zmq.java

lib_LTLIBRARIES = libjzmq.la
libjzmq_la_SOURCES = org_zmq_Zmq.h Zmq.cpp
libjzmq_la_CXXFLAGS = -I$(top_builddir)/libzmq -I$(top_srcdir)/libzmq \
@JAVA_INCLUDE@ -I$(top_builddir)/libjzmq -I$(top_srcdir)/libjzmq -Wall
libjzmq_la_LDFLAGS = -version-info 0:0:0
libjzmq_la_LIBADD = $(top_builddir)/libzmq/libzmq.la

BUILT_SOURCES = org_zmq_Zmq.h ./org/zmq/Zmq.class

$(srcdir)/Zmq.cpp: org_zmq_Zmq.h

org_zmq_Zmq.h: ./org/zmq/Zmq.class
	$(CLASSPATH_ENV) $(JAVAH) -jni -classpath ./ org.zmq.Zmq

./org/zmq/Zmq.class: classdist_noinst.stamp

dist-hook:
		-rm $(distdir)/*.h
