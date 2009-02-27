INCLUDES        = -I$(top_builddir) -I$(top_builddir)/libzmq \
                -I$(top_srcdir) -I$(top_srcdir)/libzmq

noinst_PROGRAMS = send_requests intermediate receive_replies component1 \
    component2

send_requests_SOURCES = send_requests.cpp 
send_requests_LDADD = $(top_builddir)/libzmq/libzmq.la
send_requests_CXXFLAGS = -Wall -pedantic -Werror

intermediate_SOURCES = intermediate.cpp
intermediate_LDADD = $(top_builddir)/libzmq/libzmq.la
intermediate_CXXFLAGS = -Wall -pedantic -Werror

receive_replies_SOURCES = receive_replies.cpp 
receive_replies_LDADD = $(top_builddir)/libzmq/libzmq.la
receive_replies_CXXFLAGS = -Wall -pedantic -Werror

component1_SOURCES = component1.cpp 
component1_LDADD = $(top_builddir)/libzmq/libzmq.la
component1_CXXFLAGS = -Wall -pedantic -Werror

component2_SOURCES = component2.cpp 
component2_LDADD = $(top_builddir)/libzmq/libzmq.la
component2_CXXFLAGS = -Wall -pedantic -Werror
