#ifndef __ZMQ_DECLSPEC_EXPORT_HPP_INCLUDED__
#define __ZMQ_DECLSPEC_EXPORT_HPP_INCLUDED__

#ifdef ZMQ_HAVE_WINDOWS
#define declspec_export  __declspec(dllexport)
#else 
#define declspec_export 
#endif
#endif
