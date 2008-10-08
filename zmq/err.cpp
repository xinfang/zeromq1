#include "err.hpp"
#include <winsock2.h>

namespace zmq
{

const char * wsa_error()
{
    int my_error_code = WSAGetLastError();\
    const char *err_msg = NULL;
    if (my_error_code!=WSAEWOULDBLOCK) {\
    		
    	
        err_msg = 
        (my_error_code == WSABASEERR )? "No Error" : 
        (my_error_code == WSAEINTR )? "Interrupted system call" : 
        (my_error_code == WSAEBADF )? "Bad file number" : 
        (my_error_code == WSAEACCES )? "Permission denied" : 
        (my_error_code == WSAEFAULT )? "Bad address" : 
        (my_error_code == WSAEINVAL )? "Invalid argument" : 
        (my_error_code == WSAEMFILE )? "Too many open files" : 
        (my_error_code == WSAEWOULDBLOCK )? "Operation would block" : 
        (my_error_code == WSAEINPROGRESS )? "Operation now in progress" : 
        (my_error_code == WSAEALREADY )? "Operation already in progress" : 
        (my_error_code == WSAENOTSOCK )? "Socket operation on non-socket" : 
        (my_error_code == WSAEDESTADDRREQ )? "Destination address required" : 
        (my_error_code == WSAEMSGSIZE )? "Message too long" : 
        (my_error_code == WSAEPROTOTYPE )? "Protocol wrong type for socket" : 
        (my_error_code == WSAENOPROTOOPT )? "Bad protocol option" : 
        (my_error_code == WSAEPROTONOSUPPORT )? "Protocol not supported" : 
        (my_error_code == WSAESOCKTNOSUPPORT )? "Socket type not supported" : 
        (my_error_code == WSAEOPNOTSUPP )? "Operation not supported on socket" : 
        (my_error_code == WSAEPFNOSUPPORT )? "Protocol family not supported" : 
        (my_error_code == WSAEAFNOSUPPORT )? "Address family not supported by protocol family" : 
        (my_error_code == WSAEADDRINUSE )? "Address already in use" : 
        (my_error_code == WSAEADDRNOTAVAIL )? "Can't assign requested address" : 
        (my_error_code == WSAENETDOWN )? "Network is down" : 
        (my_error_code == WSAENETUNREACH )? "Network is unreachable" : 
        (my_error_code == WSAENETRESET )? "Net dropped connection or reset" : 
        (my_error_code == WSAECONNABORTED )? "Software caused connection abort" : 
        (my_error_code == WSAECONNRESET )? "Connection reset by peer" : 
        (my_error_code == WSAENOBUFS )? "No buffer space available" : 
        (my_error_code == WSAEISCONN )? "Socket is already connected" : 
        (my_error_code == WSAENOTCONN )? "Socket is not connected" : 
        (my_error_code == WSAESHUTDOWN )? "Can't send after socket shutdown" : 
        (my_error_code == WSAETOOMANYREFS )? "Too many references can't splice" : 
        (my_error_code == WSAETIMEDOUT )? "Connection timed out" : 
        (my_error_code == WSAECONNREFUSED )? "Connection refused" : 
        (my_error_code == WSAELOOP )? "Too many levels of symbolic links" : 
        (my_error_code == WSAENAMETOOLONG )? "File name too long" : 
        (my_error_code == WSAEHOSTDOWN )? "Host is down" : 
        (my_error_code == WSAEHOSTUNREACH )? "No Route to Host" : 
        (my_error_code == WSAENOTEMPTY )? "Directory not empty" : 
        (my_error_code == WSAEPROCLIM )? "Too many processes" : 
        (my_error_code == WSAEUSERS )? "Too many users" : 
        (my_error_code == WSAEDQUOT )? "Disc Quota Exceeded" : 
        (my_error_code == WSAESTALE )? "Stale NFS file handle" : 
        (my_error_code == WSAEREMOTE )? "Too many levels of remote in path" : 
        (my_error_code == WSASYSNOTREADY )? "Network SubSystem is unavailable" : 
        (my_error_code == WSAVERNOTSUPPORTED )? "WINSOCK DLL Version out of range" : 
        (my_error_code == WSANOTINITIALISED )? "Successful WSASTARTUP not yet performed" : 
        (my_error_code == WSAHOST_NOT_FOUND )? "Host not found" : 
        (my_error_code == WSATRY_AGAIN )? "Non-Authoritative Host not found" : 
        (my_error_code == WSANO_RECOVERY )? "Non-Recoverable errors: FORMERR REFUSED NOTIMP" : 
        (my_error_code == WSANO_DATA )? "Valid name no data record of requested": "error not defined"; 
     
        
        }
        return err_msg;
}

}