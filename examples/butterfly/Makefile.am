INCLUDES        = -I$(top_builddir) -I$(top_builddir)/libzmq \
                -I$(top_srcdir) -I$(top_srcdir)/libzmq

noinst_PROGRAMS = send_requests queue receive_replies do_a do_b

send_requests_SOURCES = send_requests.cpp 
send_requests_LDADD = $(top_builddir)/libzmq/libzmq.la
send_requests_CXXFLAGS = -Wall -pedantic -Werror

queue_SOURCES = queue.cpp
queue_LDADD = $(top_builddir)/libzmq/libzmq.la
queue_CXXFLAGS = -Wall -pedantic -Werror

receive_replies_SOURCES = receive_replies.cpp 
receive_replies_LDADD = $(top_builddir)/libzmq/libzmq.la
receive_replies_CXXFLAGS = -Wall -pedantic -Werror

do_a_SOURCES = do_a.cpp 
do_a_LDADD = $(top_builddir)/libzmq/libzmq.la
do_a_CXXFLAGS = -Wall -pedantic -Werror

do_b_SOURCES = do_b.cpp 
do_b_LDADD = $(top_builddir)/libzmq/libzmq.la
do_b_CXXFLAGS = -Wall -pedantic -Werror
