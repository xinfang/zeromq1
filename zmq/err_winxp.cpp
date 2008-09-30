#include "err_winxp.hpp"
#include <winsock2.h>

const char *getErrorMessage(int myErrorCode)
{

const char *errMsg = 
(myErrorCode == WSABASEERR )? "No Error" : 
(myErrorCode == WSAEINTR )? "Interrupted system call" : 
(myErrorCode == WSAEBADF )? "Bad file number" : 
(myErrorCode == WSAEACCES )? "Permission denied" : 
(myErrorCode == WSAEFAULT )? "Bad address" : 
(myErrorCode == WSAEINVAL )? "Invalid argument" : 
(myErrorCode == WSAEMFILE )? "Too many open files" : 
(myErrorCode == WSAEWOULDBLOCK )? "Operation would block" : 
(myErrorCode == WSAEINPROGRESS )? "Operation now in progress" : 
(myErrorCode == WSAEALREADY )? "Operation already in progress" : 
(myErrorCode == WSAENOTSOCK )? "Socket operation on non-socket" : 
(myErrorCode == WSAEDESTADDRREQ )? "Destination address required" : 
(myErrorCode == WSAEMSGSIZE )? "Message too long" : 
(myErrorCode == WSAEPROTOTYPE )? "Protocol wrong type for socket" : 
(myErrorCode == WSAENOPROTOOPT )? "Bad protocol option" : 
(myErrorCode == WSAEPROTONOSUPPORT )? "Protocol not supported" : 
(myErrorCode == WSAESOCKTNOSUPPORT )? "Socket type not supported" : 
(myErrorCode == WSAEOPNOTSUPP )? "Operation not supported on socket" : 
(myErrorCode == WSAEPFNOSUPPORT )? "Protocol family not supported" : 
(myErrorCode == WSAEAFNOSUPPORT )? "Address family not supported by protocol family" : 
(myErrorCode == WSAEADDRINUSE )? "Address already in use" : 
(myErrorCode == WSAEADDRNOTAVAIL )? "Can't assign requested address" : 
(myErrorCode == WSAENETDOWN )? "Network is down" : 
(myErrorCode == WSAENETUNREACH )? "Network is unreachable" : 
(myErrorCode == WSAENETRESET )? "Net dropped connection or reset" : 
(myErrorCode == WSAECONNABORTED )? "Software caused connection abort" : 
(myErrorCode == WSAECONNRESET )? "Connection reset by peer" : 
(myErrorCode == WSAENOBUFS )? "No buffer space available" : 
(myErrorCode == WSAEISCONN )? "Socket is already connected" : 
(myErrorCode == WSAENOTCONN )? "Socket is not connected" : 
(myErrorCode == WSAESHUTDOWN )? "Can't send after socket shutdown" : 
(myErrorCode == WSAETOOMANYREFS )? "Too many references can't splice" : 
(myErrorCode == WSAETIMEDOUT )? "Connection timed out" : 
(myErrorCode == WSAECONNREFUSED )? "Connection refused" : 
(myErrorCode == WSAELOOP )? "Too many levels of symbolic links" : 
(myErrorCode == WSAENAMETOOLONG )? "File name too long" : 
(myErrorCode == WSAEHOSTDOWN )? "Host is down" : 
(myErrorCode == WSAEHOSTUNREACH )? "No Route to Host" : 
(myErrorCode == WSAENOTEMPTY )? "Directory not empty" : 
(myErrorCode == WSAEPROCLIM )? "Too many processes" : 
(myErrorCode == WSAEUSERS )? "Too many users" : 
(myErrorCode == WSAEDQUOT )? "Disc Quota Exceeded" : 
(myErrorCode == WSAESTALE )? "Stale NFS file handle" : 
(myErrorCode == WSAEREMOTE )? "Too many levels of remote in path" : 
(myErrorCode == WSASYSNOTREADY )? "Network SubSystem is unavailable" : 
(myErrorCode == WSAVERNOTSUPPORTED )? "WINSOCK DLL Version out of range" : 
(myErrorCode == WSANOTINITIALISED )? "Successful WSASTARTUP not yet performed" : 
(myErrorCode == WSAHOST_NOT_FOUND )? "Host not found" : 
(myErrorCode == WSATRY_AGAIN )? "Non-Authoritative Host not found" : 
(myErrorCode == WSANO_RECOVERY )? "Non-Recoverable errors: FORMERR REFUSED NOTIMP" : 
(myErrorCode == WSANO_DATA )? "Valid name no data record of requested": "error not defined"; 
return errMsg;

}

