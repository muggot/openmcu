/*
 * machdep.h
 *
 * Unix machine dependencies
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pmachdep.h,v $
 * Revision 1.71  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.70  2005/11/25 00:06:12  csoutheren
 * Applied patch #1364593 from Hannes Friederich
 * Also changed so PTimesMutex is no longer descended from PSemaphore on
 * non-Windows platforms
 *
 * Revision 1.69  2005/08/04 20:10:24  csoutheren
 * Apply patch #1217596
 * Fixed problems with MacOSX Tiger
 * Thanks to Hannes Friederich
 *
 * Revision 1.68  2005/08/04 19:46:51  csoutheren
 * Applied patch #1240770
 * Fixed problem with compilation under Solaris 10
 * Thanks to Boris Pavacic
 *
 * Revision 1.67  2004/11/16 00:30:38  csoutheren
 * Added Cygwin support
 *
 * Revision 1.66  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.65  2004/06/17 23:37:04  csoutheren
 * Added definition of upad128_t for Solaris
 *
 * Revision 1.64  2004/05/14 05:23:39  ykiryanov
 * Added stl header
 *
 * Revision 1.63  2004/04/18 05:45:55  ykiryanov
 * Added TCP_NODELAY definition and removed BE_BONELESS. BeOS is boned now
 *
 * Revision 1.62  2004/04/02 03:32:11  ykiryanov
 * Added prototypes for missing dl*() functions
 *
 * Revision 1.61  2004/02/23 20:00:15  ykiryanov
 * Fixed a typo in declaration of setegid(0
 *
 * Revision 1.60  2004/02/22 04:33:19  ykiryanov
 * Added missing prototype for setegid for BeOS
 *
 * Revision 1.59  2004/02/22 03:27:30  ykiryanov
 * Added missing prototype for seteuid for BeOS
 *
 * Revision 1.58  2004/02/21 21:26:30  ykiryanov
 * Added P_THREADIDENTIFIER for BeOS threads
 *
 * Revision 1.57  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.56  2003/05/06 06:59:12  robertj
 * Dynamic library support for MacOSX, thanks Hugo Santos
 *
 * Revision 1.55  2003/04/23 00:35:47  craigs
 * Fixed problem with pmachdep.h and MacOSX thanks to Hugo Santos
 *
 * Revision 1.54  2003/01/24 10:21:06  robertj
 * Fixed issues in RTEMS support, thanks Vladimir Nesic
 *
 * Revision 1.53  2002/12/02 03:57:18  robertj
 * More RTEMS support patches, thank you Vladimir Nesic.
 *
 * Revision 1.52  2002/11/22 10:14:07  robertj
 * QNX port, thanks Xiaodan Tang
 *
 * Revision 1.51  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.50  2002/10/16 11:29:05  rogerh
 * remove redundant #include.
 *
 * Revision 1.49  2002/10/14 22:41:35  rogerh
 * Wrap _THREAD_SAFE to remove a warning if is already defined.
 *
 * Revision 1.48  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.47  2002/06/27 07:51:48  robertj
 * GNU 3.1 compatibility under Solaris
 *
 * Revision 1.46  2001/10/11 02:20:54  robertj
 * Added IRIX support (no audio/video), thanks Andre Schulze.
 *
 * Revision 1.45  2001/08/11 07:57:30  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.44  2001/06/30 06:59:06  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.43  2001/03/07 06:54:56  yurik
 * Changed email to current one
 *
 * Revision 1.42  2001/02/23 08:05:19  rogerh
 * Add tcp.h for MACOS_X (Darwin)
 *
 * Revision 1.41  2001/01/16 11:52:46  rogerh
 * Add patch from Jac Goudsmit <jac_goudsmit@yahoo.com> for BONE in BeOS 5
 *
 * Revision 1.40  2000/06/21 01:01:21  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.39  2000/05/12 01:37:38  rogerh
 * Add netinet/tcp.h to NetBSD and OpenBSD too.
 *
 * Revision 1.38  2000/05/05 07:08:01  robertj
 * Same again for FreeBSD and Solaris
 *
 * Revision 1.37  2000/05/05 05:06:38  robertj
 * Added tcp.h to get TCP_NODELAY for setsockopt call.
 *
 * Revision 1.36  2000/04/10 11:39:49  rogerh
 * Add NetBSD pthread support
 *
 * Revision 1.35  2000/04/09 18:29:02  rogerh
 * Add my NetBSD changes
 *
 * Revision 1.34  2000/04/07 05:45:49  rogerh
 * Add extra include to Mac OS X section
 *
 * Revision 1.33  2000/04/06 11:05:32  rogerh
 * Add MACOSX support from Kevin Packard
 *
 * Revision 1.32  2000/04/03 22:03:01  rogerh
 * Fix socklen_t usage for FreeBSD 3.x
 *
 * Revision 1.31  2000/03/08 12:17:09  rogerh
 * Add OpenBSD support
 *
 * Revision 1.30  2000/01/25 04:55:36  robertj
 * Added FreeBSD support for distinction between v3.x and later versions. Thanks Roger Hardiman.
 *
 * Revision 1.29  2000/01/20 08:20:57  robertj
 * FreeBSD v3 compatibility changes, thanks Roger Hardiman & Motonori Shindo
 *
 * Revision 1.28  1999/08/09 01:51:42  robertj
 * Added missing paths include (esp. for VARRUN in svcproc)
 *
 * Revision 1.27  1999/07/16 10:21:51  robertj
 * Added paths header file so _PATH_VARRUN is defined for svcprox.
 *
 * Revision 1.26  1999/03/02 05:41:58  robertj
 * More BeOS changes
 *
 * Revision 1.25  1999/02/26 04:10:39  robertj
 * More BeOS port changes
 *
 * Revision 1.24  1999/02/22 13:26:53  robertj
 * BeOS port changes.
 *
 * Revision 1.23  1999/02/06 05:49:44  robertj
 * BeOS port effort by Yuri Kiryanov <openh323@kiryanov.com>
 *
 * Revision 1.22  1999/01/08 01:28:16  robertj
 * Added pthreads support for FreeBSD
 *
 * Revision 1.21  1999/01/07 03:37:15  robertj
 * dded default for pthreads, shortens command line in compile.
 *
 * Revision 1.20  1998/12/21 06:47:20  robertj
 * Solaris 5.7 support.
 *
 * Revision 1.19  1998/12/04 12:38:07  robertj
 * Fixed signal include
 *
 * Revision 1.18  1998/11/24 09:38:19  robertj
 * FreeBSD port.
 *
 * Revision 1.17  1998/11/14 01:08:25  robertj
 * PPC linux GNU compatibility.
 *
 * Revision 1.16  1998/09/24 04:11:45  robertj
 * Added open software license.
 *
 */

#ifndef _PMACHDEP_H
#define _PMACHDEP_H

///////////////////////////////////////////////////////////////////////////////
#if defined(P_LINUX)

#include <paths.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/termios.h>
#include <unistd.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <dlfcn.h>

#define HAS_IFREQ
#define PSETPGRP()  setpgrp()

#if __GNU_LIBRARY__ < 6
#define P_LINUX_LIB_OLD
typedef int socklen_t;
#endif

#ifdef PPC
typedef size_t socklen_t;
#endif

///////////////////////////////////////////////////////////////////////////////
#elif defined(P_FREEBSD)

#if defined(P_PTHREADS)
#ifndef _THREAD_SAFE
#define _THREAD_SAFE
#endif
#define P_THREAD_SAFE_CLIB

#include <pthread.h>
#endif

#include <paths.h>
#include <errno.h>
#include <dlfcn.h>
#include <termios.h>
#include <sys/fcntl.h>
#include <sys/filio.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/signal.h>
#include <net/if.h>
#include <netinet/tcp.h>

/* socklen_t is defined in FreeBSD 3.4-STABLE, 4.0-RELEASE and above */
#if (P_FREEBSD <= 340000)
typedef int socklen_t;
#endif

#define HAS_IFREQ

#define PSETPGRP()  setpgrp(0, 0)

///////////////////////////////////////////////////////////////////////////////
#elif defined(P_OPENBSD)

#if defined(P_PTHREADS)
#define _THREAD_SAFE
#define P_THREAD_SAFE_CLIB

#include <pthread.h>
#endif

#include <paths.h>
#include <errno.h>
#include <dlfcn.h>
#include <termios.h>
#include <sys/fcntl.h>
#include <sys/filio.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <net/if.h>
#include <netinet/tcp.h>

#define HAS_IFREQ

#define PSETPGRP()  setpgrp(0, 0)

///////////////////////////////////////////////////////////////////////////////
#elif defined(P_NETBSD)

#if defined(P_PTHREADS)
#define _THREAD_SAFE
#define P_THREAD_SAFE_CLIB

#include <pthread.h>
#endif

#include <stdlib.h>
#include <paths.h>
#include <errno.h>
#include <dlfcn.h>
#include <termios.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/filio.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <net/if.h>
#include <netinet/tcp.h>

#define HAS_IFREQ

#define PSETPGRP()  setpgrp(0, 0)

///////////////////////////////////////////////////////////////////////////////
#elif defined(P_SOLARIS)

#include <errno.h>
#include <sys/sockio.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <dlfcn.h>
#include <net/if.h>
#include <sys/sockio.h>

#if !defined(P_HAS_UPAD128_T)
typedef union {
  long double _q;
  uint32_t _l[4];
} upad128_t;
#endif

#define PSETPGRP()  setpgrp()

#define INADDR_NONE     -1
#if P_SOLARIS < 7
typedef int socklen_t;
#endif

#define HAS_IFREQ

#if __GNUC__ < 3
extern "C" {

int ftime (struct timeb *);
pid_t wait3(int *status, int options, struct rusage *rusage);
int gethostname(char *, int);

};
#endif

///////////////////////////////////////////////////////////////////////////////
#elif defined (P_SUN4)

#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <net/if.h>
#include <sys/sockio.h>

#define HAS_IFREQ
#define PSETPGRP()  setpgrp(0, 0)
#define raise(s)    kill(getpid(),s)

extern "C" {

char *mktemp(char *);
int accept(int, struct sockaddr *, int *);
int connect(int, struct sockaddr *, int);
int ioctl(int, int, void *);
int recv(int, void *, int, int);
int recvfrom(int, void *, int, int, struct sockaddr *, int *);
int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
int sendto(int, const void *, int, int, const struct sockaddr *, int);
int send(int, const void *, int, int);
int shutdown(int, int);
int socket(int, int, int);
int vfork();
void bzero(void *, int);
void closelog();
void gettimeofday(struct timeval * tv, struct timezone * tz);
void openlog(const char *, int, int);
void syslog(int, char *, ...);
int setpgrp(int, int);
pid_t wait3(int *status, int options, struct rusage *rusage);
int bind(int, struct sockaddr *, int);
int listen(int, int);
int getsockopt(int, int, int, char *, int *);
int setsockopt(int, int, int, char *, int);
int getpeername(int, struct sockaddr *, int *);
int gethostname(char *, int);
int getsockname(int, struct sockaddr *, int *);
char * inet_ntoa(struct in_addr);

int ftime (struct timeb *);

struct hostent * gethostbyname(const char *);
struct hostent * gethostbyaddr(const char *, int, int);
struct servent * getservbyname(const char *, const char *);

#include <sys/termios.h>
#undef NL0
#undef NL1
#undef CR0
#undef CR1
#undef CR2
#undef CR3
#undef TAB0
#undef TAB1
#undef TAB2
#undef XTABS
#undef BS0
#undef BS1
#undef FF0
#undef FF1
#undef ECHO
#undef NOFLSH
#undef TOSTOP
#undef FLUSHO
#undef PENDIN
};


///////////////////////////////////////////////////////////////////////////////
#elif __BEOS__

#include <errno.h>
#include <termios.h>
#include <sys/socket.h>
#include <OS.h>
#include <cpp/stl.h>

#define SOCK_RAW 3 // raw-protocol interface, not suported in R4
#define PF_INET AF_INET
#define TCP_NODELAY 1
typedef int socklen_t;
#include <bone/arpa/inet.h>

#define wait3(s, o, r) waitpid(-1, s, o)
#define PSETPGRP()  setpgid(0,0)
int seteuid(uid_t euid);
int setegid(gid_t gid);

#define RTLD_NOW        0x2
extern "C" {
void *dlopen(const char *path, int mode);
int dlclose(void *handle);
void *dlsym(void *handle, const char *symbol);
};

///////////////////////////////////////////////////////////////////////////////
#elif defined (P_MACOSX) || defined(P_MACOS)
 
#if defined(P_PTHREADS)
#   define _THREAD_SAFE
#   define P_THREAD_SAFE_CLIB
#   include <pthread.h>
#endif
#if defined(P_MAC_MPTHREADS)
#include <CoreServices/CoreServices.h>
// Blasted Mac <CoreServices.h> comes with 17 years of crufty history
// crapping up the namespace, thankyouverymuch.  (What I really want is
// just Multiprocessing.h, but that drags in nearly as much crap and isn't
// readily available on Mac OS X.)
// So:  undefine the troublespots as they occur.
#undef nil // you morons.

// Open Transport and UNIX networking headers don't get along.  Why did
// Apple have to do this?  And what's worse, they are functionally equivalent
// #defines, Apple could have easily made the headers compatible.  But no.
#undef TCP_NODELAY
#undef TCP_MAXSEG
#endif // MPThreads

#include <paths.h>
#include <errno.h>
#include <termios.h>
#include <sys/fcntl.h>
#include <sys/filio.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/signal.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>

#if defined (P_MACOSX) && (P_MACOSX < 800)
typedef int socklen_t;
#endif
 
#define HAS_IFREQ
 
#define PSETPGRP()  setpgrp(0, 0)


///////////////////////////////////////////////////////////////////////////////
#elif defined (P_AIX)

#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <dlfcn.h>
#include <net/if.h>
#include <strings.h>

#define HAS_IFREQ

#define PSETPGRP()  setpgrp()

///////////////////////////////////////////////////////////////////////////////
#elif defined (P_IRIX)

#include <errno.h>
#include <sys/sockio.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <dlfcn.h>
#include <net/if.h>
#include <sys/sockio.h>

typedef int socklen_t;

#define PSETPGRP()  setpgrp()

///////////////////////////////////////////////////////////////////////////////
#elif defined (P_VXWORKS)

#include <taskLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <time.h>
#include <ioLib.h>
#include <unistd.h>
#include <selectLib.h>
#include <inetLib.h>
#include <hostLib.h>
#include <ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <socklib.h>
#include <signal.h>

// Prevent conflict between net/mbuf.h and some ASN.1 header files
// VxWorks uses some #define m_data <to-something-else> constructions
#undef m_data
#undef m_type

#define HAS_IFREQ

#define _exit(i)   exit(i)

typedef int socklen_t;

extern int h_errno;

#define SUCCESS    0
#define NOTFOUND   1

struct hostent * Vx_gethostbyname(char *name, struct hostent *hp);
struct hostent * Vx_gethostbyaddr(char *name, struct hostent *hp);

#define strcasecmp strcmp

#define P_HAS_SEMAPHORES
#define _THREAD_SAFE
#define P_THREAD_SAFE_CLIB
#define P_THREADIDENTIFIER long


///////////////////////////////////////////////////////////////////////////////
#elif defined(P_RTEMS)

#include <sys/termios.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <net/if.h>
typedef int socklen_t;
typedef int64_t         quad_t;
extern "C" {
  int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *tv);
  int strcasecmp(const char *, const char *);
  int strncasecmp(const char *, const char *, size_t);
  char* strdup(const char *);
}
#define PSETPGRP()  tcsetprgrp(0, 0)
#define wait3(s, o, r) waitpid(-1, s, o)
#define seteuid setuid
#define setegid setgid
#define HAS_IFREQ

///////////////////////////////////////////////////////////////////////////////
#elif defined(P_QNX)

#if defined(P_PTHREADS)
#define _THREAD_SAFE
#define P_THREAD_SAFE_CLIB

#include <pthread.h>
#include <resolv.h> /* for pthread's h_errno */
#endif

#include <stdlib.h>
#include <paths.h>
#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/termio.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/tcp.h>

#define HAS_IFREQ
#define PSETPGRP()  setpgrp()

///////////////////////////////////////////////////////////////////////////////
#elif defined(P_CYGWIN)
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

///////////////////////////////////////////////////////////////////////////////

// Other operating systems here

#else
#endif

///////////////////////////////////////////////////////////////////////////////

// includes common to all Unix variants

#include <netdb.h>
#include <dirent.h>
#include <limits.h>

#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#ifndef P_VXWORKS
#include <sys/time.h>
#endif

#ifndef __BEOS__
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

typedef int SOCKET;

#ifdef P_PTHREADS

#include <pthread.h>
#define P_THREADIDENTIFIER pthread_t

#if defined(P_HAS_SEMAPHORES) || defined(P_HAS_NAMED_SEMAPHORES)
#include <semaphore.h>
#endif  // P_HAS_SEMPAHORES

#endif  // P_PTHREADS

#ifdef BE_THREADS
#define P_THREADIDENTIFIER thread_id
#endif // BE_THREADS

#endif // _PMACHDEP_H

// End of file
