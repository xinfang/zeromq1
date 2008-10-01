#ifndef __ZMQ_STDINT_HPP_INCLUDED__
#define __ZMQ_STDINT_HPP_INCLUDED__

#ifdef ZMQ_HAVE_WINXP

typedef __int8 uint8_t;
typedef __int16 uint16_t;
typedef __int32 uint32_t;
typedef __int64 uint64_t;

#else
#include "stdintgen.hpp"
#endif

#endif