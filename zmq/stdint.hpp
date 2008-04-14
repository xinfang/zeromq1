/* Define to 1 if you have the <stdint.h> header file. */
#define ZMQ_HAVE_STDINT_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define ZMQ_HAVE_INTTYPES_H 1

#if ZMQ_HAVE_STDINT_H
    #include <stdint.h>
#elif ZMQ_HAVE_INTTYPES_H
    #include <inttypes.h>
#endif

