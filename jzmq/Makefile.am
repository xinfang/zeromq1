jzmqdir = /tmp

jzmq_JAVA = Jzmq.java 
EXTRA_DIST = $(jzmq_JAVA)
CLASSES = $(jzmq_JAVA:.java=.class)

lib_LTLIBRARIES = libjzmq.la
libjzmq_la_SOURCES = Jzmq.h Jzmq.cpp
libjzmq_la_CXXFLAGS = -I$(JAVA_HOME)/include -I $(JAVA_HOME)/include/linux -Wall -pedantic
libjzmq_la_LDFLAGS = -version-info 0:0:0
libjzmq_la_LIBADD = $(top_builddir)/zmq/libzmq.la

BUILT_SOURCES = Jzmq.h $(CLASSES)

$(srcdir)/Jzmq.cpp: Jzmq.h

Jzmq.h: $(CLASSES)
	$(CLASSPATH_ENV) $(JAVAH) Jzmq

# force build of class files
$(CLASSES): classjzmq.stamp

dist-hook:
		-rm $(distdir)/Jzmq.h
