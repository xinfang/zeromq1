/* libzmq/zmq/platform.hpp.  Generated from platform.hpp.in by configure.  */
/* Have Linux OS */
/* #undef ZMQ_HAVE_LINUX */

/* Have DarwinOSX OS */
/* #undef ZMQ_HAVE_OSX */

/* Have Solaris OS */
/* #undef ZMQ_HAVE_SOLARIS */

/* Have OpenVMS OS */
#define ZMQ_HAVE_OPENVMS 1

/* Have FreeBSD OS */
/* #undef ZMQ_HAVE_FREEBSD */

/* Have OpenBSD OS */
/* #undef ZMQ_HAVE_OPENBSD */

/* Have QNX Neutrino OS */
/* #undef ZMQ_HAVE_QNXNTO */

/* Have AIX OS */
/* #undef ZMQ_HAVE_AIX */

/* Have HPUX OS */
/* #undef ZMQ_HAVE_HPUX */

/* Have Windows OS */
/* #undef ZMQ_HAVE_WINDOWS */

/* Have MinGW32 */
/* #undef ZMQ_HAVE_MINGW32 */

/* Have SCTP extension */
/* #undef ZMQ_HAVE_SCTP */

/* Have OpenPGM extension */
/* #undef ZMQ_HAVE_OPENPGM */

/* Have eventfd extension. */
/* #undef ZMQ_HAVE_EVENTFD */

/* Have AMQP extension. */
#define ZMQ_HAVE_AMQP 1

#ifdef ZMQ_HAVE_HPUX
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#ifdef ZMQ_HAVE_OPENVMS
#define socklen_t unsigned
typedef long long int64_t;
#endif

/* WindowsXP and above */
#ifdef ZMQ_HAVE_MINGW32
#define WINVER 0x0501
#endif
