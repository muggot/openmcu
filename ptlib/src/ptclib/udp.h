//
// stund/udp.h
// 
// Copyright (c) 2002 Alan Hawrylyshen
// 
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


/*************************** UDP networking stuff  *******************/
#ifndef udp_h
#define udp_h

#ifdef WIN32

#include <errno.h>

#ifndef _WIN32_WCE
#include <winsock2.h>
#else
#include <winsock.h>
#endif

#include <io.h>

typedef int socklen_t;
#ifndef errno
#define errno WSAGetLastError()
#endif
typedef SOCKET Socket;

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

typedef LONGLONG Int64; 

#else

typedef int Socket;
static const Socket INVALID_SOCKET = -1;
static const int SOCKET_ERROR = -1;


int closesocket( Socket fd );

#ifdef P_RTEMS
typedef int socklen_t;
extern "C" {
  int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *tv);
}
#endif

#endif



const int udpMaxMessageLength=16*1024;

/// Take a name in the form "host.foo.com:5000" and return ip and port
void 
parseHostName( char* peerName, unsigned int* ip, unsigned short* portVal, unsigned int defaultPort = 10000 );

/// Open a UDP socket to receive on the given port - if port is 0, pick a a port, if interfaceIp!=0 then use ONLY the interface specified instead of all of them 
Socket
openPort( unsigned short port=0, unsigned int interfaceIp=0 );

/// recive a UDP message 
bool 
getMessage( Socket fd, char* buf, int* len, unsigned int* srcIp, unsigned short* srcPort );

/// send a UDP message 
bool 
sendMessage( Socket fd, char* msg, int len, 
             unsigned int dstIp=0, unsigned short dstPort=0 );

/// set up network - does nothing in unix but needed for windows
void
initNetwork();


// Local Variables:
// mode:c++
// c-file-style:"bsd"
// c-file-offsets:((case-label . +))
// c-basic-offset:4
// indent-tabs-mode:nil
// End:
#endif
