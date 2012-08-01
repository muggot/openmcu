/*
 * socket.cxx
 *
 * Berkley sockets classes implementation
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
 * $Log: socket.cxx,v $
 * Revision 1.120  2007/06/27 03:15:21  rjongbloed
 * Added ability to select filtering of down network interfaces.
 *
 * Revision 1.119  2007/02/17 18:57:58  hfriederich
 * Use similar code in IsLocalHost() as in GetInterfaceTable() to fix
 * incorrect IsLocalHost() on Mac OS X. Untested on other platforms!
 *
 * Revision 1.118  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.117  2006/02/21 13:57:31  csoutheren
 * Second attempt at fixing problem with interfaces having multiple addresses
 *
 * Revision 1.116  2006/02/18 15:57:45  dsandras
 * Applied patch from Richard van der Hoff and Stephane Epardaud <stef lunatech
 * com> to fix infinite loop with IPv6 interfaces. Thanks!
 *
 * Revision 1.115  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.114  2005/11/23 11:47:03  shorne
 * Changed EnableQoS to EnableGQoS
 *
 * Revision 1.113  2005/07/15 12:45:13  rogerhardiman
 * Fix bug 1237508 (M Zygmuntowicz). Make IPV6 code use #if instead of #ifdef.
 *
 * Revision 1.112  2005/07/13 11:48:55  csoutheren
 * Backported QOS changes from isvo branch
 *
 * Revision 1.111  2005/02/07 12:12:31  csoutheren
 * Expanded interface list routines to include IPV6 addresses
 * Added IPV6 to GetLocalAddress
 *
 * Revision 1.110  2004/08/24 07:08:11  csoutheren
 * Added use of recvmsg to determine which interface UDP packets arrive on
 *
 * Revision 1.109  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.108  2004/07/08 00:57:29  csoutheren
 * Added check for EINTR on connect
 * Thanks to Alex Vishnev
 *
 * Revision 1.107  2004/07/03 23:50:42  csoutheren
 * Removed warnings under Solaris
 *
 * Revision 1.106  2004/05/06 11:28:30  rjongbloed
 * Changed P_fd_set to use malloc/free isntead of new/delete due to pedantry about [].
 *
 * Revision 1.105  2004/05/05 06:52:37  ykiryanov
 * Made BeOS changes
 *
 * Revision 1.104  2004/04/27 04:37:51  rjongbloed
 * Fixed ability to break of a PSocket::Select call under linux when a socket
 *   is closed by another thread.
 *
 * Revision 1.103  2004/04/24 06:28:12  rjongbloed
 * Fixed GCC 3.4.0 warnings about PAssertNULL and improved recoverability on
 *   NULL pointer usage in various bits of code.
 *
 * Revision 1.102  2004/04/18 07:46:32  rjongbloed
 * Fixed other unix builds after Yuri's BeOS changes.
 *
 * Revision 1.101  2004/04/18 00:21:35  ykiryanov
 * Cleaned up BeOS related code. Less ifdefs, more functionality
 *
 * Revision 1.100  2003/10/27 04:06:14  csoutheren
 * Added code to allow compilation of new QoS code on Unix
 *
 * Revision 1.99  2003/01/24 10:21:06  robertj
 * Fixed issues in RTEMS support, thanks Vladimir Nesic
 *
 * Revision 1.98  2002/11/22 10:14:07  robertj
 * QNX port, thanks Xiaodan Tang
 *
 * Revision 1.97  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.96  2002/10/22 10:25:07  rogerh
 * Fix process_rtentry() following Thomas's patch.
 *
 * Revision 1.95  2002/10/22 07:42:52  robertj
 * Added extra debugging for file handle and thread leak detection.
 *
 * Revision 1.94  2002/10/22 06:53:38  craigs
 * Fixed signed/unsigned problem in GetRoutTable thanks to Thomas Jalsovsky
 *
 * Revision 1.93  2002/10/19 06:12:20  robertj
 * Moved P_fd_set::Zero() from platform independent to platform dependent
 *   code as Win32 implementation is completely different from Unix.
 *
 * Revision 1.92  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.91  2002/10/17 12:57:24  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.90  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.89  2002/10/08 14:31:43  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.88  2002/06/05 12:29:16  craigs
 * Changes for gcc 3.1
 *
 * Revision 1.87  2002/04/18 06:16:06  robertj
 * Fixed Net BSD problem with RTF_CLONED flag, thanks Motoyuki OHMORI
 * Fixed operator precedence problem with bit mask tests in RTF_CLONE code.
 *
 * Revision 1.86  2002/04/12 07:57:41  robertj
 * Fixed bug introduced into Accept() by previous change.
 *
 * Revision 1.85  2002/04/12 01:42:41  robertj
 * Changed return value on os_connect() and os_accept() to make sure
 *   get the correct error codes propagated up under unix.
 *
 * Revision 1.84  2002/02/13 02:19:47  robertj
 * Fixed mistake in previous patch, is if not ifdef!
 *
 * Revision 1.83  2002/02/13 00:50:32  robertj
 * Fixed use of symbol in older versionsof Solaris, thanks Markus Storm
 *
 * Revision 1.82  2002/01/31 22:52:18  robertj
 * Added fix for buffer too small in Solaris GetRouteTable(), thanks Markus Storm.
 *
 * Revision 1.81  2001/12/17 23:33:50  robertj
 * Solaris 8 porting changes, thanks James Dugal
 *
 * Revision 1.80  2001/12/10 07:07:27  rogerh
 * Take out some #includes which were already in pachdep.h. Fixes openBSD 2.9
 *
 * Revision 1.79  2001/11/22 12:29:57  rogerh
 * Take out the cloned flag on OpenBSD so it compiles
 *
 * Revision 1.78  2001/11/14 10:37:32  rogerh
 * Define _SIZEOF_ADDR_IFREQ as OpenBSD does not have it
 *
 * Revision 1.77  2001/10/28 23:00:10  robertj
 * Fixed Solaris and IRIX compatibility issue, thanks Andre Schulze
 *
 * Revision 1.76  2001/10/11 02:20:54  robertj
 * Added IRIX support (no audio/video), thanks Andre Schulze.
 *
 * Revision 1.75  2001/10/03 19:31:56  rogerh
 * Add MAC OS X support to GetInterfaceTable
 *
 * Revision 1.74  2001/10/03 10:18:15  rogerh
 * Make Mac OS X (Darwin) use the new GetRouteTable() function.
 *
 * Revision 1.73  2001/09/24 15:37:35  rogerh
 * Add GetRouteTable() for BSD Unix. Based on FreeBSD's own networking code,
 * and from an implementation by Martin Nilsson <martin@gneto.com>
 *
 * Revision 1.72  2001/09/19 00:41:20  robertj
 * Fixed GetInterfaceTable so does not add duplicate interfaces into list.
 * Changed the loop condition to allow for BSD variable length records.
 *
 * Revision 1.71  2001/09/18 05:56:03  robertj
 * Fixed numerous problems with thread suspend/resume and signals handling.
 *
 * Revision 1.70  2001/09/10 03:03:36  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.69  2001/08/16 11:58:22  rogerh
 * Add more Mac OS X changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.68  2001/08/12 07:12:40  rogerh
 * More Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.67  2001/08/12 06:34:33  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.66  2001/08/07 02:27:22  robertj
 * Fixed some incorrect error values returned in Read() and Write() functions.
 *
 * Revision 1.65  2001/07/03 04:41:25  yurik
 * Corrections to Jac's submission from 6/28
 *
 * Revision 1.64  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.63  2001/06/19 12:09:13  rogerh
 * Mac OS X change
 *
 * Revision 1.62  2001/04/16 22:46:22  craigs
 * Fixed problem with os_connect not correctly reporting errors
 *
 * Revision 1.61  2001/03/26 03:31:53  robertj
 * Fixed Solaris compile error.
 *
 * Revision 1.60  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.59  2001/03/07 23:37:59  robertj
 * Fixed slow down in UDP packet send, thanks Dmitriy Reka
 *
 * Revision 1.58  2001/03/07 06:56:36  yurik
 * Made adjustment for BONE platforms as requested by Jac Goudsmit
 *
 * Revision 1.57  2001/03/06 00:16:59  robertj
 * Fixed BSD compatibility problem.
 *
 * Revision 1.56  2001/03/05 04:28:56  robertj
 * Added net mask to interface info returned by GetInterfaceTable()
 *
 * Revision 1.55  2001/02/02 23:31:30  robertj
 * Fixed enumeration of interfaces, thanks Bertrand Croq.
 *
 * Revision 1.54  2001/01/17 03:48:25  rogerh
 * Fix GetInterfaceTable so it actually works through all interfaces rather
 * than falling over after the first entry.
 *
 * Revision 1.53  2001/01/16 12:56:01  rogerh
 * On BeOS sa_data is 'unsigned char *'. Linux and BSD defines sa_data as 'char *'
 * Add typecast, submitted by Jac Goudsmit <jac_goudsmit@yahoo.com>
 *
 * Revision 1.52  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.51  2000/04/19 00:13:53  robertj
 * BeOS port changes.
 *
 * Revision 1.50  2000/04/07 05:43:48  rogerh
 * Fix a compilation error in a non-pthreaded function. Found by Kevin Packard
 *
 * Revision 1.49  2000/03/17 03:45:40  craigs
 * Fixed problem with connect call hanging
 *
 * Revision 1.48  2000/02/17 23:47:40  robertj
 * Fixed error in check for SIOCGHWADDR define, thanks Markus Storm.
 *
 * Revision 1.47  2000/01/20 08:20:57  robertj
 * FreeBSD v3 compatibility changes, thanks Roger Hardiman & Motonori Shindo
 *
 * Revision 1.46  1999/11/18 13:45:21  craigs
 * Removed obsolete declaration of iostream semaphore
 *
 * Revision 1.45  1999/10/30 13:43:01  craigs
 * Added correct method of aborting socket operations asynchronously
 *
 * Revision 1.44  1999/09/27 01:04:42  robertj
 * BeOS support changes.
 *
 * Revision 1.43  1999/09/12 07:06:23  craigs
 * Added support for getting Solaris interface info
 *
 * Revision 1.42  1999/09/10 02:31:19  craigs
 * Added interface table routines
 *
 * Revision 1.41  1999/09/03 02:26:25  robertj
 * Changes to aid in breaking I/O locks on thread termination. Still needs more work esp in BSD!
 *
 * Revision 1.40  1999/05/01 03:52:20  robertj
 * Fixed various egcs warnings.
 *
 * Revision 1.39  1999/03/02 05:41:59  robertj
 * More BeOS changes
 *
 * Revision 1.38  1999/02/26 04:10:39  robertj
 * More BeOS port changes
 *
 * Revision 1.37  1999/02/22 13:26:54  robertj
 * BeOS port changes.
 *
 * Revision 1.36  1998/11/30 21:51:58  robertj
 * New directory structure.
 *
 * Revision 1.35  1998/11/24 09:39:22  robertj
 * FreeBSD port.
 *
 * Revision 1.34  1998/11/22 08:11:37  craigs
 * *** empty log message ***
 *
 * Revision 1.33  1998/11/14 10:37:38  robertj
 * Changed semantics of os_sendto to return TRUE if ANY bytes are sent.
 *
 * Revision 1.32  1998/10/16 01:16:55  craigs
 * Added Yield to help with cooperative multithreading.
 *
 * Revision 1.31  1998/10/11 02:23:16  craigs
 * Fixed problem with socket writes not correctly detecting EOF
 *
 * Revision 1.30  1998/09/24 08:21:11  robertj
 * Fixed warning on GNU 6 library.
 *
 * Revision 1.29  1998/09/24 07:55:51  robertj
 * Fixed warning on solaris build.
 *
 * Revision 1.28  1998/09/24 04:13:49  robertj
 * Added open software license.
 *
 * Revision 1.27  1998/09/18 05:46:00  robertj
 * Fixed incorrectly returning success on a connect() error other than a timeout.
 *
 * Revision 1.26  1998/09/08 11:31:51  robertj
 * Fixed ippp bug on very full packets.
 *
 * Revision 1.25  1998/09/08 09:54:31  robertj
 * Fixed ppp and ippp compatibility.
 *
 * Revision 1.24  1998/09/08 05:15:14  robertj
 * Fixed problem in Windows requiring snmpapi.dll for PEthSocket class.
 *
 * Revision 1.23  1998/08/27 01:13:20  robertj
 * Changes to resolve signedness in GNU C library v6
 * Remove Linux EthSocket stuff from Sun build, still needs implementing.
 *
 * Revision 1.22  1998/08/21 05:30:59  robertj
 * Ethernet socket implementation.
 *
 */

#pragma implementation "sockets.h"
#pragma implementation "socket.h"
#pragma implementation "ipsock.h"
#pragma implementation "udpsock.h"
#pragma implementation "tcpsock.h"
#pragma implementation "ipdsock.h"
#pragma implementation "ethsock.h"
#pragma implementation "qos.h"


#include <ptlib.h>
#include <ptlib/sockets.h>

#include <map>
#include <ptlib/pstring.h>

#if defined(SIOCGENADDR)
#define SIO_Get_MAC_Address SIOCGENADDR
#define  ifr_macaddr         ifr_ifru.ifru_enaddr
#elif defined(SIOCGIFHWADDR)
#define SIO_Get_MAC_Address SIOCGIFHWADDR
#define  ifr_macaddr         ifr_hwaddr.sa_data
#endif

#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD) || defined(P_SOLARIS) || defined(P_MACOSX) || defined(P_MACOS) || defined(P_IRIX) || defined(P_VXWORKS) || defined(P_RTEMS) || defined(P_QNX)
#define ifr_netmask ifr_addr

#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>

#include <netinet/in.h>
#if !defined(P_QNX)
#include <netinet/if_ether.h>
#endif

#define ROUNDUP(a) \
        ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))

#endif

#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD) || defined(P_MACOSX) || defined(P_MACOS) || defined(P_QNX)
#include <sys/sysctl.h>
#endif

#ifdef P_RTEMS
#include <bsp.h>
#endif

#ifdef __BEOS__
#include <posix/sys/ioctl.h> // for FIONBIO
#include <be/bone/net/if.h> // for ifconf
#include <be/bone/sys/sockio.h> // for SIOCGI*
#endif

#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD) || defined(P_MACOSX) || defined(P_VXWORKS) || defined(P_RTEMS) || defined(P_QNX)
// Define _SIZEOF_IFREQ for platforms (eg OpenBSD) which do not have it.
#ifndef _SIZEOF_ADDR_IFREQ
#define _SIZEOF_ADDR_IFREQ(ifr) \
  ((ifr).ifr_addr.sa_len > sizeof(struct sockaddr) ? \
  (sizeof(struct ifreq) - sizeof(struct sockaddr) + \
  (ifr).ifr_addr.sa_len) : sizeof(struct ifreq))
#endif
#endif

int PX_NewHandle(const char *, int);

#ifdef P_VXWORKS
// VxWorks variant of inet_ntoa() allocates INET_ADDR_LEN bytes via malloc
// BUT DOES NOT FREE IT !!!  Use inet_ntoa_b() instead.
#define INET_ADDR_LEN      18
extern "C" void inet_ntoa_b(struct in_addr inetAddress, char *pString);
#endif // P_VXWORKS

//////////////////////////////////////////////////////////////////////////////
// P_fd_set

void P_fd_set::Construct()
{
  max_fd = PProcess::Current().GetMaxHandles();
  set = (fd_set *)malloc((max_fd+7)>>3);
}


void P_fd_set::Zero()
{
  if (PAssertNULL(set) != NULL)
    memset(set, 0, (max_fd+7)>>3);
}


//////////////////////////////////////////////////////////////////////////////

PSocket::~PSocket()
{
  os_close();
}

int PSocket::os_close()
{
  if (os_handle < 0)
    return -1;

  // send a shutdown to the other end
  ::shutdown(os_handle, 2);

  return PXClose();
}


static int SetNonBlocking(int fd)
{
  if (fd < 0)
    return -1;

  // Set non-blocking so we can use select calls to break I/O block on close
  int cmd = 1;
#if defined(P_VXWORKS)
  if (::ioctl(fd, FIONBIO, &cmd) == 0)
#else
  if (::ioctl(fd, FIONBIO, &cmd) == 0 && ::fcntl(fd, F_SETFD, 1) == 0)
#endif
    return fd;

  ::close(fd);
  return -1;
}


int PSocket::os_socket(int af, int type, int protocol)
{
  // attempt to create a socket
  return SetNonBlocking(PX_NewHandle(GetClass(), ::socket(af, type, protocol)));
}


BOOL PSocket::os_connect(struct sockaddr * addr, PINDEX size)
{
  int val;
  do {
    val = ::connect(os_handle, addr, size);
  } while (val != 0 && errno == EINTR);
  if (val == 0 || errno != EINPROGRESS)
    return ConvertOSError(val);

  if (!PXSetIOBlock(PXConnectBlock, readTimeout))
    return FALSE;

  // A successful select() call does not necessarily mean the socket connected OK.
  int optval = -1;
  socklen_t optlen = sizeof(optval);
  getsockopt(os_handle, SOL_SOCKET, SO_ERROR, (char *)&optval, &optlen);
  if (optval != 0) {
    errno = optval;
    return ConvertOSError(-1);
  }

  return TRUE;
}


BOOL PSocket::os_accept(PSocket & listener, struct sockaddr * addr, PINDEX * size)
{
  if (!listener.PXSetIOBlock(PXAcceptBlock, listener.GetReadTimeout()))
    return SetErrorValues(listener.GetErrorCode(), listener.GetErrorNumber());

#if defined(E_PROTO)
  for (;;) {
    int new_fd = ::accept(listener.GetHandle(), addr, (socklen_t *)size);
    if (new_fd >= 0)
      return ConvertOSError(os_handle = SetNonBlocking(new_fd));

    if (errno != EPROTO)
      return ConvertOSError(-1);

    PTRACE(3, "PWLib\tAccept on " << sock << " failed with EPROTO - retrying");
  }
#else
  return ConvertOSError(os_handle = SetNonBlocking(::accept(listener.GetHandle(), addr, (socklen_t *)size)));
#endif
}


#if !defined(P_PTHREADS) && !defined(P_MAC_MPTHREADS) && !defined(__BEOS__)

PChannel::Errors PSocket::Select(SelectList & read,
                                 SelectList & write,
                                 SelectList & except,
      const PTimeInterval & timeout)
{
  PINDEX i, j;
  PINDEX nextfd = 0;
  int maxfds = 0;
  Errors lastError = NoError;
  PThread * unblockThread = PThread::Current();
  
  P_fd_set fds[3];
  SelectList * list[3] = { &read, &write, &except };

  for (i = 0; i < 3; i++) {
    for (j = 0; j < list[i]->GetSize(); j++) {
      PSocket & socket = (*list[i])[j];
      if (!socket.IsOpen())
        lastError = NotOpen;
      else {
        int h = socket.GetHandle();
        fds[i] += h;
        if (h > maxfds)
          maxfds = h;
      }
      socket.px_selectMutex.Wait();
      socket.px_selectThread = unblockThread;
    }
  }

  if (lastError == NoError) {
    P_timeval tval = timeout;
    int result = ::select(maxfds+1, 
                          (fd_set *)fds[0], 
                          (fd_set *)fds[1], 
                          (fd_set *)fds[2], 
                          tval);

    int osError;
    (void)ConvertOSError(result, lastError, osError);
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < list[i]->GetSize(); j++) {
      PSocket & socket = (*list[i])[j];
      socket.px_selectThread = NULL;
      socket.px_selectMutex.Signal();
      if (lastError == NoError) {
        int h = socket.GetHandle();
        if (h < 0)
          lastError = Interrupted;
        else if (!fds[i].IsPresent(h))
          list[i]->RemoveAt(j--);
      }
    }
  }

  return lastError;
}
                     
#else

PChannel::Errors PSocket::Select(SelectList & read,
                                 SelectList & write,
                                 SelectList & except,
                                 const PTimeInterval & timeout)
{
  PINDEX i, j;
  int maxfds = 0;
  Errors lastError = NoError;
  PThread * unblockThread = PThread::Current();
  int unblockPipe = unblockThread->unblockPipe[0];

  P_fd_set fds[3];
  SelectList * list[3] = { &read, &write, &except };

  for (i = 0; i < 3; i++) {
    for (j = 0; j < list[i]->GetSize(); j++) {
      PSocket & socket = (*list[i])[j];
      if (!socket.IsOpen())
        lastError = NotOpen;
      else {
        int h = socket.GetHandle();
        fds[i] += h;
        if (h > maxfds)
          maxfds = h;
      }
      socket.px_selectMutex.Wait();
      socket.px_selectThread = unblockThread;
    }
  }

  int result = -1;
  if (lastError == NoError) {
    fds[0] += unblockPipe;
    if (unblockPipe > maxfds)
      maxfds = unblockPipe;

    P_timeval tval = timeout;
    do {
      result = ::select(maxfds+1, (fd_set *)fds[0], (fd_set *)fds[1], (fd_set *)fds[2], tval);
    } while (result < 0 && errno == EINTR);

    int osError;
    if (ConvertOSError(result, lastError, osError)) {
      if (fds[0].IsPresent(unblockPipe)) {
        PTRACE(6, "PWLib\tSelect unblocked fd=" << unblockPipe);
        BYTE ch;
        ::read(unblockPipe, &ch, 1);
        lastError = Interrupted;
      }
    }
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < list[i]->GetSize(); j++) {
      PSocket & socket = (*list[i])[j];
      socket.px_selectThread = NULL;
      socket.px_selectMutex.Signal();
      if (lastError == NoError) {
        int h = socket.GetHandle();
        if (h < 0)
          lastError = Interrupted;
        else if (!fds[i].IsPresent(h))
          list[i]->RemoveAt(j--);
      }
    }
  }

  return lastError;
}

#endif


PIPSocket::Address::Address(DWORD dw)
{
  operator=(dw);
}


PIPSocket::Address & PIPSocket::Address::operator=(DWORD dw)
{
  if (dw == 0) {
    version = 0;
    memset(&v, 0, sizeof(v));
  }
  else {
    version = 4;
    v.four.s_addr = dw;
  }

  return *this;
}


PIPSocket::Address::operator DWORD() const
{
  return version != 4 ? 0 : (DWORD)v.four.s_addr;
}

BYTE PIPSocket::Address::Byte1() const
{
  return *(((BYTE *)&v.four.s_addr)+0);
}

BYTE PIPSocket::Address::Byte2() const
{
  return *(((BYTE *)&v.four.s_addr)+1);
}

BYTE PIPSocket::Address::Byte3() const
{
  return *(((BYTE *)&v.four.s_addr)+2);
}

BYTE PIPSocket::Address::Byte4() const
{
  return *(((BYTE *)&v.four.s_addr)+3);
}

PIPSocket::Address::Address(BYTE b1, BYTE b2, BYTE b3, BYTE b4)
{
  version = 4;
  BYTE * p = (BYTE *)&v.four.s_addr;
  p[0] = b1;
  p[1] = b2;
  p[2] = b3;
  p[3] = b4;
}

BOOL PIPSocket::IsLocalHost(const PString & hostname)
{
  if (hostname.IsEmpty())
    return TRUE;

  if (hostname *= "localhost")
    return TRUE;

  // lookup the host address using inet_addr, assuming it is a "." address
  Address addr = hostname;
  if (addr.IsLoopback())  // Is 127.0.0.1
    return TRUE;
  if (!addr.IsValid())
    return FALSE;

  if (!GetHostAddress(hostname, addr))
    return FALSE;

#if P_HAS_IPV6
  {
    FILE * file;
    int dummy;
    int addr6[16];
    char ifaceName[255];
    BOOL found = FALSE;
    if ((file = fopen("/proc/net/if_inet6", "r")) != NULL) {
      while (!found && (fscanf(file, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x %x %x %x %x %255s\n",
              &addr6[0],  &addr6[1],  &addr6[2],  &addr6[3], 
              &addr6[4],  &addr6[5],  &addr6[6],  &addr6[7], 
              &addr6[8],  &addr6[9],  &addr6[10], &addr6[11], 
              &addr6[12], &addr6[13], &addr6[14], &addr6[15], 
             &dummy, &dummy, &dummy, &dummy, ifaceName) != EOF)) {
        Address ip6addr(
          psprintf("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
              addr6[0],  addr6[1],  addr6[2],  addr6[3], 
              addr6[4],  addr6[5],  addr6[6],  addr6[7], 
              addr6[8],  addr6[9],  addr6[10], addr6[11], 
              addr6[12], addr6[13], addr6[14], addr6[15]
          )
        );
        found = (ip6addr *= addr);
      }
      fclose(file);
    }
    if (found)
      return TRUE;
  }
#endif

  // check IPV4 addresses
  PUDPSocket sock;
  
  PBYTEArray buffer;
  struct ifconf ifConf;

#ifdef SIOCGIFNUM
  int ifNum;
  PAssert(::ioctl(sock.GetHandle(), SIOCGIFNUM, &ifNum) >= 0, "could not do ioctl for ifNum");
  ifConf.ifc_len = ifNum * sizeof(ifreq);
#else
  ifConf.ifc_len = 100 * sizeof(ifreq); // That's a LOT of interfaces!
#endif

  ifConf.ifc_req = (struct ifreq *)buffer.GetPointer(ifConf.ifc_len);
  
  if (ioctl(sock.GetHandle(), SIOCGIFCONF, &ifConf) >= 0) {
    void * ifEndList = (char *)ifConf.ifc_req + ifConf.ifc_len;
    ifreq * ifName = ifConf.ifc_req;

    while (ifName < ifEndList) {
      struct ifreq ifReq;
      memcpy(&ifReq, ifName, sizeof(ifreq));
      
      if (ioctl(sock.GetHandle(), SIOCGIFFLAGS, &ifReq) >= 0) {
        int flags = ifReq.ifr_flags;
        if ((flags & IFF_UP) && ioctl(sock.GetHandle(), SIOCGIFADDR, &ifReq) >= 0) {
          sockaddr_in * sin = (sockaddr_in *)&ifReq.ifr_addr;
          PIPSocket::Address address = sin->sin_addr;
          if (addr *= address)
            return TRUE;
        }
      }
      
#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD) || defined(P_MACOSX) || defined(P_VXWORKS) || defined(P_RTEMS) || defined(P_QNX)
      // move the ifName pointer along to the next ifreq entry
      ifName = (struct ifreq *)((char *)ifName + _SIZEOF_ADDR_IFREQ(*ifName));
#else
      ifName++;
#endif
    }
  }
  
  return FALSE;
}


////////////////////////////////////////////////////////////////
//
//  PTCPSocket
//
BOOL PTCPSocket::Read(void * buf, PINDEX maxLen)

{
  lastReadCount = 0;

  // wait until select indicates there is data to read, or until
  // a timeout occurs
  if (!PXSetIOBlock(PXReadBlock, readTimeout))
    return FALSE;

  // attempt to read out of band data
  char buffer[32];
  int ooblen;
  while ((ooblen = ::recv(os_handle, buffer, sizeof(buffer), MSG_OOB)) > 0) 
    OnOutOfBand(buffer, ooblen);

  // attempt to read non-out of band data
  int r = ::recv(os_handle, (char *)buf, maxLen, 0);
  if (!ConvertOSError(r, LastReadError))
    return FALSE;

  lastReadCount = r;
  return lastReadCount > 0;
}


#if P_HAS_RECVMSG

int PSocket::os_recvfrom(
      void * buf,     // Data to be written as URGENT TCP data.
      PINDEX len,     // Number of bytes pointed to by <CODE>buf</CODE>.
      int    flags,
      sockaddr * addr, // Address from which the datagram was received.
      PINDEX * addrlen)
{
  lastReadCount = 0;

  if (!PXSetIOBlock(PXReadBlock, readTimeout))
    return FALSE;

  // if we don't care what interface the packet arrives on, then don't bother getting the information
  if (!catchReceiveToAddr) {
    int r = ::recvfrom(os_handle, (char *)buf, len, flags, (sockaddr *)addr, (socklen_t *)addrlen);
    if (!ConvertOSError(r, LastReadError))
      return FALSE;

    lastReadCount = r;
    return lastReadCount > 0;
  }

  msghdr readData;
  memset(&readData, 0, sizeof(readData));

  readData.msg_name       = addr;
  readData.msg_namelen    = *addrlen;

  iovec readVector;
  readVector.iov_base     = buf;
  readVector.iov_len      = len;
  readData.msg_iov        = &readVector;
  readData.msg_iovlen     = 1;

  char auxdata[50];
  readData.msg_control    = auxdata;
  readData.msg_controllen = sizeof(auxdata);

  // read a packet 
  int r = ::recvmsg(os_handle, &readData, 0);
  if (!ConvertOSError(r, LastReadError))
    return FALSE;

  lastReadCount = r;

  if (r >= 0) {
    struct cmsghdr * cmsg;
    for (cmsg = CMSG_FIRSTHDR(&readData); cmsg != NULL; cmsg = CMSG_NXTHDR(&readData,cmsg)) {
      if (cmsg->cmsg_level == SOL_IP && cmsg->cmsg_type == IP_PKTINFO) {
        in_pktinfo * info = (in_pktinfo *)CMSG_DATA(cmsg);
        SetLastReceiveAddr(&info->ipi_spec_dst, sizeof(in_addr));
        break;
      }
    }
  }

  return lastReadCount > 0;
}

#else

BOOL PSocket::os_recvfrom(
      void * buf,     // Data to be written as URGENT TCP data.
      PINDEX len,     // Number of bytes pointed to by <CODE>buf</CODE>.
      int    flags,
      sockaddr * addr, // Address from which the datagram was received.
      PINDEX * addrlen)
{
  lastReadCount = 0;

  if (!PXSetIOBlock(PXReadBlock, readTimeout))
    return FALSE;

  // attempt to read non-out of band data
  int r = ::recvfrom(os_handle, (char *)buf, len, flags, (sockaddr *)addr, (socklen_t *)addrlen);
  if (!ConvertOSError(r, LastReadError))
    return FALSE;

  lastReadCount = r;
  return lastReadCount > 0;
}

#endif


BOOL PSocket::os_sendto(
      const void * buf,   // Data to be written as URGENT TCP data.
      PINDEX len,         // Number of bytes pointed to by <CODE>buf</CODE>.
      int flags,
      sockaddr * addr, // Address to which the datagram is sent.
      PINDEX addrlen)  
{
  lastWriteCount = 0;

  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF, LastWriteError);

  // attempt to read data
  int result;
  for (;;) {
    if (addr != NULL)
      result = ::sendto(os_handle, (char *)buf, len, flags, (sockaddr *)addr, addrlen);
    else
      result = ::send(os_handle, (char *)buf, len, flags);

    if (result > 0)
      break;

    if (errno != EWOULDBLOCK)
      return ConvertOSError(-1, LastWriteError);

    if (!PXSetIOBlock(PXWriteBlock, writeTimeout))
      return FALSE;
  }

#if !defined(P_PTHREADS) && !defined(P_MAC_MPTHREADS)
  PThread::Yield(); // Starvation prevention
#endif

  lastWriteCount = result;
  return ConvertOSError(0, LastWriteError);
}


BOOL PSocket::Read(void * buf, PINDEX len)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF, LastReadError);

  if (!PXSetIOBlock(PXReadBlock, readTimeout)) 
    return FALSE;

  if (ConvertOSError(lastReadCount = ::recv(os_handle, (char *)buf, len, 0)))
    return lastReadCount > 0;

  lastReadCount = 0;
  return FALSE;
}



//////////////////////////////////////////////////////////////////
//
//  PEthSocket
//

PEthSocket::PEthSocket(PINDEX, PINDEX, PINDEX)
{
  medium = MediumUnknown;
  filterMask = FilterDirected|FilterBroadcast;
  filterType = TypeAll;
  fakeMacHeader = FALSE;
  ipppInterface = FALSE;
}


PEthSocket::~PEthSocket()
{
  Close();
}


BOOL PEthSocket::Connect(const PString & interfaceName)
{
  Close();

  fakeMacHeader = FALSE;
  ipppInterface = FALSE;

  if (strncmp("eth", interfaceName, 3) == 0)
    medium = Medium802_3;
  else if (strncmp("lo", interfaceName, 2) == 0)
    medium = MediumLoop;
  else if (strncmp("sl", interfaceName, 2) == 0) {
    medium = MediumWan;
    fakeMacHeader = TRUE;
  }
  else if (strncmp("ppp", interfaceName, 3) == 0) {
    medium = MediumWan;
    fakeMacHeader = TRUE;
  }
  else if (strncmp("ippp", interfaceName, 4) == 0) {
    medium = MediumWan;
    ipppInterface = TRUE;
  }
#ifdef P_RTEMS
  else if (strncmp(RTEMS_BSP_NETWORK_DRIVER_NAME, interfaceName, 3) == 0)
    medium = Medium802_3;
#endif
  else
    return SetErrorValues(NotFound, ENOENT);

#if defined(SIO_Get_MAC_Address) 
  PUDPSocket ifsock;
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strcpy(ifr.ifr_name, interfaceName);
  if (!ConvertOSError(ioctl(ifsock.GetHandle(), SIO_Get_MAC_Address, &ifr)))
    return FALSE;

  memcpy(&macAddress, ifr.ifr_macaddr, sizeof(macAddress));
#endif

  channelName = interfaceName;
  return OpenSocket();
}


BOOL PEthSocket::OpenSocket()
{
#ifdef SOCK_PACKET
  if (!ConvertOSError(os_handle = os_socket(AF_INET, SOCK_PACKET, htons(filterType))))
    return FALSE;

  struct sockaddr addr;
  memset(&addr, 0, sizeof(addr));
  addr.sa_family = AF_INET;
  strcpy(addr.sa_data, channelName);
  if (!ConvertOSError(bind(os_handle, &addr, sizeof(addr)))) {
    os_close();
    os_handle = -1;
    return FALSE;
  }
#endif

  return TRUE;
}


BOOL PEthSocket::Close()
{
  SetFilter(FilterDirected, filterType);  // Turn off promiscuous mode
  return PSocket::Close();
}


BOOL PEthSocket::EnumInterfaces(PINDEX idx, PString & name)
{
  PUDPSocket ifsock;

  ifreq ifreqs[20]; // Maximum of 20 interfaces
  struct ifconf ifc;
  ifc.ifc_len = sizeof(ifreqs);
  ifc.ifc_buf = (caddr_t)ifreqs;
  if (!ConvertOSError(ioctl(ifsock.GetHandle(), SIOCGIFCONF, &ifc)))
    return FALSE;

  int ifcount = ifc.ifc_len/sizeof(ifreq);
  int ifidx;
  for (ifidx = 0; ifidx < ifcount; ifidx++) {
    if (strchr(ifreqs[ifidx].ifr_name, ':') == NULL) {
      ifreq ifr;
      strcpy(ifr.ifr_name, ifreqs[ifidx].ifr_name);
      if (ioctl(ifsock.GetHandle(), SIOCGIFFLAGS, &ifr) == 0 &&
          (ifr.ifr_flags & IFF_UP) != 0 &&
           idx-- == 0) {
        name = ifreqs[ifidx].ifr_name;
        return TRUE;
      }
    }
  }

  return FALSE;
}


BOOL PEthSocket::GetAddress(Address & addr)
{
  if (!IsOpen())
    return FALSE;

  addr = macAddress;
  return TRUE;
}


BOOL PEthSocket::EnumIpAddress(PINDEX idx,
                               PIPSocket::Address & addr,
                               PIPSocket::Address & net_mask)
{
  if (!IsOpen())
    return FALSE;

  PUDPSocket ifsock;
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  if (idx == 0)
    strcpy(ifr.ifr_name, channelName);
  else
    sprintf(ifr.ifr_name, "%s:%u", (const char *)channelName, (int)(idx-1));
  if (!ConvertOSError(ioctl(os_handle, SIOCGIFADDR, &ifr)))
    return FALSE;

  sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
  addr = sin->sin_addr;

  if (!ConvertOSError(ioctl(os_handle, SIOCGIFNETMASK, &ifr)))
    return FALSE;

  net_mask = sin->sin_addr;
  return TRUE;
}


BOOL PEthSocket::GetFilter(unsigned & mask, WORD & type)
{
  if (!IsOpen())
    return FALSE;

  ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strcpy(ifr.ifr_name, channelName);
  if (!ConvertOSError(ioctl(os_handle, SIOCGIFFLAGS, &ifr)))
    return FALSE;

  if ((ifr.ifr_flags&IFF_PROMISC) != 0)
    filterMask |= FilterPromiscuous;
  else
    filterMask &= ~FilterPromiscuous;

  mask = filterMask;
  type = filterType;
  return TRUE;
}


BOOL PEthSocket::SetFilter(unsigned filter, WORD type)
{
  if (!IsOpen())
    return FALSE;

  if (filterType != type) {
    os_close();
    filterType = type;
    if (!OpenSocket())
      return FALSE;
  }

  ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strcpy(ifr.ifr_name, channelName);
  if (!ConvertOSError(ioctl(os_handle, SIOCGIFFLAGS, &ifr)))
    return FALSE;

  if ((filter&FilterPromiscuous) != 0)
    ifr.ifr_flags |= IFF_PROMISC;
  else
    ifr.ifr_flags &= ~IFF_PROMISC;

  if (!ConvertOSError(ioctl(os_handle, SIOCSIFFLAGS, &ifr)))
    return FALSE;

  filterMask = filter;

  return TRUE;
}


PEthSocket::MediumTypes PEthSocket::GetMedium()
{
  return medium;
}


BOOL PEthSocket::ResetAdaptor()
{
  // No implementation
  return TRUE;
}


BOOL PEthSocket::Read(void * buf, PINDEX len)
{
  static const BYTE macHeader[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 8, 0 };

  BYTE * bufptr = (BYTE *)buf;

  if (fakeMacHeader) {
    if (len <= (PINDEX)sizeof(macHeader)) {
      memcpy(bufptr, macHeader, len);
      lastReadCount = len;
      return TRUE;
    }

    memcpy(bufptr, macHeader, sizeof(macHeader));
    bufptr += sizeof(macHeader);
    len -= sizeof(macHeader);
  }

  for (;;) {
    sockaddr from;
    PINDEX fromlen = sizeof(from);
    if (!os_recvfrom(bufptr, len, 0, &from, &fromlen))
      return FALSE;

    if (channelName != from.sa_data)
      continue;

    if (ipppInterface) {
      if (lastReadCount <= 10)
        return FALSE;
      if (memcmp(bufptr+6, "\xff\x03\x00\x21", 4) != 0) {
        memmove(bufptr+sizeof(macHeader), bufptr, lastReadCount);
        lastReadCount += sizeof(macHeader);
      }
      else {
        memmove(bufptr+sizeof(macHeader), bufptr+10, lastReadCount);
        lastReadCount += sizeof(macHeader)-10;
      }
      memcpy(bufptr, macHeader, sizeof(macHeader));
      break;
    }

    if (fakeMacHeader) {
      lastReadCount += sizeof(macHeader);
      break;
    }

    if ((filterMask&FilterPromiscuous) != 0)
      break;

    if ((filterMask&FilterDirected) != 0 && macAddress == bufptr)
      break;

    static const Address broadcast;
    if ((filterMask&FilterBroadcast) != 0 && broadcast == bufptr)
      break;
  }

  return lastReadCount > 0;
}


BOOL PEthSocket::Write(const void * buf, PINDEX len)
{
  sockaddr to;
  strcpy((char *)to.sa_data, channelName);
  return os_sendto(buf, len, 0, &to, sizeof(to)) && lastWriteCount >= len;
}


///////////////////////////////////////////////////////////////////////////////

BOOL PIPSocket::GetGatewayAddress(Address & addr)
{
  RouteTable table;
  if (GetRouteTable(table)) {
    for (PINDEX i = 0; i < table.GetSize(); i++) {
      if (table[i].GetNetwork() == 0) {
        addr = table[i].GetDestination();
        return TRUE;
      }
    }
  }
  return FALSE;
}



PString PIPSocket::GetGatewayInterface()
{
  RouteTable table;
  if (GetRouteTable(table)) {
    for (PINDEX i = 0; i < table.GetSize(); i++) {
      if (table[i].GetNetwork() == 0)
        return table[i].GetInterface();
    }
  }
  return PString();
}

#if defined(P_LINUX) || defined (P_AIX)

BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
  PTextFile procfile;
  if (!procfile.Open("/proc/net/route", PFile::ReadOnly))
    return FALSE;

  for (;;) {
    // Ignore heading line or remainder of route line
    procfile.ignore(1000, '\n');
    if (procfile.eof())
      return TRUE;

    char iface[20];
    unsigned long net_addr, dest_addr, net_mask;
    int flags, refcnt, use, metric;
    procfile >> iface >> ::hex >> net_addr >> dest_addr >> flags 
                      >> ::dec >> refcnt >> use >> metric 
                      >> ::hex >> net_mask;
    if (procfile.bad())
      return FALSE;

    RouteEntry * entry = new RouteEntry(net_addr);
    entry->net_mask = net_mask;
    entry->destination = dest_addr;
    entry->interfaceName = iface;
    entry->metric = metric;
    table.Append(entry);
  }
}

#elif defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD) || defined(P_MACOSX) || defined(P_QNX) 

BOOL process_rtentry(struct rt_msghdr *rtm, char *ptr, unsigned long *p_net_addr,
                     unsigned long *p_net_mask, unsigned long *p_dest_addr, int *p_metric);
BOOL get_ifname(int index, char *name);

BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
  int mib[6];
  size_t space_needed;
  char *limit, *buf, *ptr;
  struct rt_msghdr *rtm;

  InterfaceTable if_table;


  // Read the Routing Table
  mib[0] = CTL_NET;
  mib[1] = PF_ROUTE;
  mib[2] = 0;
  mib[3] = 0;
  mib[4] = NET_RT_DUMP;
  mib[5] = 0;

  if (sysctl(mib, 6, NULL, &space_needed, NULL, 0) < 0) {
    printf("sysctl: net.route.0.0.dump estimate");
    return FALSE;
  }

  if ((buf = (char *)malloc(space_needed)) == NULL) {
    printf("malloc(%lu)", (unsigned long)space_needed);
    return FALSE;
  }

  // read the routing table data
  if (sysctl(mib, 6, buf, &space_needed, NULL, 0) < 0) {
    printf("sysctl: net.route.0.0.dump");
    free(buf);
    return FALSE;
  }


  // Read the interface table
  if (!GetInterfaceTable(if_table)) {
    printf("Interface Table Invalid\n");
    return FALSE;
  }


  // Process the Routing Table data
  limit = buf + space_needed;
  for (ptr = buf; ptr < limit; ptr += rtm->rtm_msglen) {

    unsigned long net_addr, dest_addr, net_mask;
    int metric;
    char name[16];

    rtm = (struct rt_msghdr *)ptr;

    if ( process_rtentry(rtm,ptr, &net_addr, &net_mask, &dest_addr, &metric) ){

      RouteEntry * entry = new RouteEntry(net_addr);
      entry->net_mask = net_mask;
      entry->destination = dest_addr;
      if ( get_ifname(rtm->rtm_index,name) )
        entry->interfaceName = name;
      entry->metric = metric;
      table.Append(entry);

    } // end if

  } // end for loop

  free(buf);
  return TRUE;
}

BOOL process_rtentry(struct rt_msghdr *rtm, char *ptr, unsigned long *p_net_addr,
                     unsigned long *p_net_mask, unsigned long *p_dest_addr, int *p_metric) {

  struct sockaddr_in *sa_in;

  unsigned long net_addr, dest_addr, net_mask;
  int metric;

  sa_in = (struct sockaddr_in *)(rtm + 1);


  // Check for zero length entry
  if (rtm->rtm_msglen == 0) {
    printf("zero length message\n");
    return FALSE;
  }

  if ((~rtm->rtm_flags&RTF_LLINFO)
#if defined(P_NETBSD) || defined(P_QNX)
        && (~rtm->rtm_flags&RTF_CLONED)     // Net BSD has flag one way
#elif !defined(P_OPENBSD) && !(defined(P_FREEBSD) && P_FREEBSD >= 800059)
        && (~rtm->rtm_flags&RTF_WASCLONED)  // Free BSD/MAC has it another
#else
                                            // Open BSD does not have it at all!
#endif
     ) {

    //strcpy(name, if_table[rtm->rtm_index].GetName);

    net_addr=dest_addr=net_mask=metric=0;

    // NET_ADDR
    if(rtm->rtm_addrs&RTA_DST ) {
      if(sa_in->sin_family == AF_INET)
        net_addr = sa_in->sin_addr.s_addr;

      sa_in = (struct sockaddr_in *)((char *)sa_in + ROUNDUP(sa_in->sin_len));
    }

    // DEST_ADDR
    if(rtm->rtm_addrs&RTA_GATEWAY) {
      if(sa_in->sin_family == AF_INET)
        dest_addr = sa_in->sin_addr.s_addr;

      sa_in = (struct sockaddr_in *)((char *)sa_in + ROUNDUP(sa_in->sin_len));
    }

    // NETMASK
    if(rtm->rtm_addrs&RTA_NETMASK && sa_in->sin_len)
      net_mask = sa_in->sin_addr.s_addr;

    if( rtm->rtm_flags&RTF_HOST)
      net_mask = 0xffffffff;


    *p_metric = metric;
    *p_net_addr = net_addr;
    *p_dest_addr = dest_addr;
    *p_net_mask = net_mask;

    return TRUE;

  } else {
    return FALSE;
  }

}

BOOL get_ifname(int index, char *name) {
  int mib[6];
  size_t needed;
  char *lim, *buf, *next;
  struct if_msghdr *ifm;
  struct  sockaddr_dl *sdl;

  mib[0] = CTL_NET;
  mib[1] = PF_ROUTE;
  mib[2] = 0;
  mib[3] = AF_INET;
  mib[4] = NET_RT_IFLIST;
  mib[5] = index;

  if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0) {
    printf("ERR route-sysctl-estimate");
    return FALSE;
  }

  if ((buf = (char *)malloc(needed)) == NULL) {
    printf("ERR malloc");
    return FALSE;
  }

  if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
    printf("ERR actual retrieval of routing table");
    free(buf);
    return FALSE;
  }

  lim = buf + needed;

  next = buf;
  if (next < lim) {

    ifm = (struct if_msghdr *)next;

    if (ifm->ifm_type == RTM_IFINFO) {
      sdl = (struct sockaddr_dl *)(ifm + 1);
    } else {
      printf("out of sync parsing NET_RT_IFLIST\n");
      return FALSE;
    }
    next += ifm->ifm_msglen;

    strncpy(name, sdl->sdl_data, sdl->sdl_nlen);
    name[sdl->sdl_nlen] = '\0';

    free(buf);
    return TRUE;

  } else {
    free(buf);
    return FALSE;
  }

}


#elif defined(P_SOLARIS)

/* jpd@louisiana.edu - influenced by Merit.edu's Gated 3.6 routine: krt_rtread_sunos5.c */

#include <sys/stream.h>
#include <stropts.h>
#include <sys/tihdr.h>
#include <sys/tiuser.h>
#include <inet/common.h>
#include <inet/mib2.h>
#include <inet/ip.h>

#ifndef T_CURRENT
#define T_CURRENT       MI_T_CURRENT
#endif

BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
#define task_pagesize 512
    char buf[task_pagesize];  /* = task_block_malloc(task_pagesize);*/
    int flags;
    int j = 0;
    int  sd, i, rc;
    struct strbuf strbuf;
    struct T_optmgmt_req *tor = (struct T_optmgmt_req *) buf;
    struct T_optmgmt_ack *toa = (struct T_optmgmt_ack *) buf;
    struct T_error_ack  *tea = (struct T_error_ack *) buf;
    struct opthdr *req;

    sd = open("/dev/ip", O_RDWR);
    if (sd < 0) {
#ifdef SOL_COMPLAIN
      perror("can't open mib stream");
#endif
      goto Return;
    }

    strbuf.buf = buf;

    tor->PRIM_type = T_OPTMGMT_REQ;
    tor->OPT_offset = sizeof(struct T_optmgmt_req);
    tor->OPT_length = sizeof(struct opthdr);
    tor->MGMT_flags = T_CURRENT;
    req = (struct opthdr *) (tor + 1);
    req->level = MIB2_IP;    /* any MIB2_xxx value ok here */
    req->name = 0;
    req->len = 0;

    strbuf.len = tor->OPT_length + tor->OPT_offset;
    flags = 0;
    rc = putmsg(sd, &strbuf, (struct strbuf *) 0, flags);
    if (rc == -1) {
#ifdef SOL_COMPLAIN
      perror("putmsg(ctl)");
#endif
      goto Return;
    }
    /*
     * each reply consists of a ctl part for one fixed structure
     * or table, as defined in mib2.h.  The format is a T_OPTMGMT_ACK,
     * containing an opthdr structure.  level/name identify the entry,
     * len is the size of the data part of the message.
     */
    req = (struct opthdr *) (toa + 1);
    strbuf.maxlen = task_pagesize;
    while (++j) {
  flags = 0;
  rc = getmsg(sd, &strbuf, (struct strbuf *) 0, &flags);
  if (rc == -1) {
#ifdef SOL_COMPLAIN
    perror("getmsg(ctl)");
#endif
    goto Return;
  }
  if (rc == 0
      && strbuf.len >= (int)sizeof(struct T_optmgmt_ack)
      && toa->PRIM_type == T_OPTMGMT_ACK
      && toa->MGMT_flags == T_SUCCESS
      && req->len == 0) {
    errno = 0;    /* just to be darned sure it's 0 */
    goto Return;    /* this is EOD msg */
  }

  if (strbuf.len >= (int)sizeof(struct T_error_ack)
      && tea->PRIM_type == T_ERROR_ACK) {
      errno = (tea->TLI_error == TSYSERR) ? tea->UNIX_error : EPROTO;
#ifdef SOL_COMPLAIN
      perror("T_ERROR_ACK in mibget");
#endif
      goto Return;
  }
      
  if (rc != MOREDATA
      || strbuf.len < (int)sizeof(struct T_optmgmt_ack)
      || toa->PRIM_type != T_OPTMGMT_ACK
      || toa->MGMT_flags != T_SUCCESS) {
      errno = ENOMSG;
      goto Return;
  }

  if (req->level != MIB2_IP
#if P_SOLARIS > 7
      || req->name != MIB2_IP_ROUTE
#endif
           ) {  /* == 21 */
      /* If this is not the routing table, skip it */
    /* Note we don't bother with IPv6 (MIB2_IP6_ROUTE) ... */
      strbuf.maxlen = task_pagesize;
      do {
    rc = getmsg(sd, (struct strbuf *) 0, &strbuf, &flags);
      } while (rc == MOREDATA) ;
      continue;
  }

  strbuf.maxlen = (task_pagesize / sizeof (mib2_ipRouteEntry_t)) * sizeof (mib2_ipRouteEntry_t);
  strbuf.len = 0;
  flags = 0;
  do {
      rc = getmsg(sd, (struct strbuf * ) 0, &strbuf, &flags);
      
      switch (rc) {
      case -1:
#ifdef SOL_COMPLAIN
        perror("mibget getmsg(data) failed.");
#endif
        goto Return;

      default:
#ifdef SOL_COMPLAIN
        fprintf(stderr,"mibget getmsg(data) returned %d, strbuf.maxlen = %d, strbuf.len = %d",
            rc,
            strbuf.maxlen,
            strbuf.len);
#endif
        goto Return;

      case MOREDATA:
      case 0:
        {
    mib2_ipRouteEntry_t *rp = (mib2_ipRouteEntry_t *) strbuf.buf;
    mib2_ipRouteEntry_t *lp = (mib2_ipRouteEntry_t *) (strbuf.buf + strbuf.len);

    do {
      char name[256];
#ifdef SOL_DEBUG_RT
      printf("%s -> %s mask %s metric %d %d %d %d %d ifc %.*s type %d/%x/%x\n",
             inet_ntoa(rp->ipRouteDest),
             inet_ntoa(rp->ipRouteNextHop),
             inet_ntoa(rp->ipRouteMask),
             rp->ipRouteMetric1,
             rp->ipRouteMetric2,
             rp->ipRouteMetric3,
             rp->ipRouteMetric4,
             rp->ipRouteMetric5,
             rp->ipRouteIfIndex.o_length,
             rp->ipRouteIfIndex.o_bytes,
             rp->ipRouteType,
             rp->ipRouteInfo.re_ire_type,
             rp->ipRouteInfo.re_flags
        );
#endif
      if (rp->ipRouteInfo.re_ire_type & (IRE_BROADCAST|IRE_CACHE|IRE_LOCAL))
                    continue;
      RouteEntry * entry = new RouteEntry(rp->ipRouteDest);
      entry->net_mask = rp->ipRouteMask;
      entry->destination = rp->ipRouteNextHop;
                  unsigned len = rp->ipRouteIfIndex.o_length;
                  if (len >= sizeof(name))
                    len = sizeof(name)-1;
      strncpy(name, rp->ipRouteIfIndex.o_bytes, len);
      name[len] = '\0';
      entry->interfaceName = name;
      entry->metric =  rp->ipRouteMetric1;
      table.Append(entry);
    } while (++rp < lp) ;
        }
        break;
      }
  } while (rc == MOREDATA) ;
    }

 Return:
    i = errno;
    (void) close(sd);
    errno = i;
    /*task_block_reclaim(task_pagesize, buf);*/
    if (errno)
      return (FALSE);
    else
      return (TRUE);
}


#elif defined(P_VXWORKS)

BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
  PAssertAlways("PIPSocket::GetRouteTable()");
  for(;;){
    char iface[20];
    unsigned long net_addr, dest_addr, net_mask;
    int  metric;
    RouteEntry * entry = new RouteEntry(net_addr);
    entry->net_mask = net_mask;
    entry->destination = dest_addr;
    entry->interfaceName = iface;
    entry->metric = metric;
    table.Append(entry);
    return TRUE;
  }
}

#else // unsupported platform

#if 0 
BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
        // Most of this code came from the source code for the "route" command 
        // so it should work on other platforms too. 
        // However, it is not complete (the "address-for-interface" function doesn't exist) and not tested! 
        
        route_table_req_t reqtable; 
        route_req_t *rrtp; 
        int i,ret; 
        
        ret = get_route_table(&reqtable); 
        if (ret < 0) 
        { 
                return FALSE; 
        } 
        
        for (i=reqtable.cnt, rrtp = reqtable.rrtp;i>0;i--, rrtp++) 
        { 
                //the datalink doesn't save addresses/masks for host and default 
                //routes, so the route_req_t may not be filled out completely 
                if (rrtp->flags & RTF_DEFAULT) { 
                        //the IP default route is 0/0 
                        ((struct sockaddr_in *)&rrtp->dst)->sin_addr.s_addr = 0; 
                        ((struct sockaddr_in *)&rrtp->mask)->sin_addr.s_addr = 0; 
        
                } else if (rrtp->flags & RTF_HOST) { 
                        //host routes are addr/32 
                        ((struct sockaddr_in *)&rrtp->mask)->sin_addr.s_addr = 0xffffffff; 
                } 
        
            RouteEntry * entry = new RouteEntry(/* address_for_interface(rrtp->iface) */); 
            entry->net_mask = rrtp->mask; 
            entry->destination = rrtp->dst; 
            entry->interfaceName = rrtp->iface; 
            entry->metric = rrtp->refcnt; 
            table.Append(entry); 
        } 
        
        free(reqtable.rrtp); 
                
        return TRUE; 
#endif // 0

BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
#warning Platform requires implemetation of GetRouteTable()
  return FALSE;
}
#endif


// fe800000000000000202e3fffe1ee330 02 40 20 80     eth0
// 00000000000000000000000000000001 01 80 10 80       lo

BOOL PIPSocket::GetInterfaceTable(InterfaceTable & list, BOOL includeDown)
{
#if P_HAS_IPV6
  // build a table of IPV6 interface addresses
  typedef std::map<PString, PString> IP6ListType;
  IP6ListType ip6Ifaces;
  {
    FILE * file;
    int dummy;
    int addr[16];
    char ifaceName[255];
    if ((file = fopen("/proc/net/if_inet6", "r")) != NULL) {
      while (fscanf(file, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x %x %x %x %x %255s\n",
              &addr[0],  &addr[1],  &addr[2],  &addr[3], 
              &addr[4],  &addr[5],  &addr[6],  &addr[7], 
              &addr[8],  &addr[9],  &addr[10], &addr[11], 
              &addr[12], &addr[13], &addr[14], &addr[15], 
             &dummy, &dummy, &dummy, &dummy, ifaceName) != EOF) {
        PString addrStr(
          psprintf("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
              addr[0],  addr[1],  addr[2],  addr[3], 
              addr[4],  addr[5],  addr[6],  addr[7], 
              addr[8],  addr[9],  addr[10], addr[11], 
              addr[12], addr[13], addr[14], addr[15]
          )
        );
        PString iface(ifaceName);
        ip6Ifaces.insert(IP6ListType::value_type(ifaceName, addrStr));
      }
      fclose(file);
    }
  }
#endif

  PUDPSocket sock;

  PBYTEArray buffer;
  struct ifconf ifConf;
  

  // HERE
#if defined(SIOCGIFNUM)
  int ifNum;
  PAssert(::ioctl(sock.GetHandle(), SIOCGIFNUM, &ifNum) >= 0, "could not do ioctl for ifNum");
  ifConf.ifc_len = ifNum * sizeof(ifreq);
#else
  ifConf.ifc_len = 100 * sizeof(ifreq); // That's a LOT of interfaces!
#endif

  ifConf.ifc_req = (struct ifreq *)buffer.GetPointer(ifConf.ifc_len);

  if (ioctl(sock.GetHandle(), SIOCGIFCONF, &ifConf) >= 0) {
    void * ifEndList = (char *)ifConf.ifc_req + ifConf.ifc_len;
    ifreq * ifName = ifConf.ifc_req;
    while (ifName < ifEndList) {

      struct ifreq ifReq;
      memcpy(&ifReq, ifName, sizeof(ifreq));

      if (ioctl(sock.GetHandle(), SIOCGIFFLAGS, &ifReq) >= 0) {
        int flags = ifReq.ifr_flags;
        if (includeDown || (flags & IFF_UP) != 0) {
          PString name(ifReq.ifr_name);

          PString macAddr;
#if defined(SIO_Get_MAC_Address)
          memcpy(&ifReq, ifName, sizeof(ifreq));
          if (ioctl(sock.GetHandle(), SIO_Get_MAC_Address, &ifReq) >= 0) {
            PEthSocket::Address a((BYTE *)ifReq.ifr_macaddr);
            macAddr = (PString)a;
          }
#endif

          memcpy(&ifReq, ifName, sizeof(ifreq));
          if (ioctl(sock.GetHandle(), SIOCGIFADDR, &ifReq) >= 0) {

            sockaddr_in * sin = (sockaddr_in *)&ifReq.ifr_addr;
            PIPSocket::Address addr = sin->sin_addr;

            memcpy(&ifReq, ifName, sizeof(ifreq));
            if (ioctl(sock.GetHandle(), SIOCGIFNETMASK, &ifReq) >= 0) {
              PIPSocket::Address mask = 
#ifndef __BEOS__
    ((sockaddr_in *)&ifReq.ifr_netmask)->sin_addr;
#else
    ((sockaddr_in *)&ifReq.ifr_mask)->sin_addr;
#endif // !__BEOS__
              PINDEX i;
              for (i = 0; i < list.GetSize(); i++) {
#ifdef P_TORNADO
                if (list[i].GetName() == name &&
                    list[i].GetAddress() == addr)
                    if(list[i].GetNetMask() == mask)
#else
                if (list[i].GetName() == name &&
                    list[i].GetAddress() == addr &&
                    list[i].GetNetMask() == mask)
#endif
                  break;
              }
#if P_HAS_IPV6
              PString ip6Addr;
              IP6ListType::const_iterator r = ip6Ifaces.find(name);
              if (r != ip6Ifaces.end())
                ip6Addr = r->second;
#endif
              if (i >= list.GetSize())
                list.Append(PNEW InterfaceEntry(name, addr, mask, macAddr
#if P_HAS_IPV6
                , ip6Addr
#endif
                ));
            }
          }
        }
      }

#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD) || defined(P_MACOSX) || defined(P_VXWORKS) || defined(P_RTEMS) || defined(P_QNX)
      // move the ifName pointer along to the next ifreq entry
      ifName = (struct ifreq *)((char *)ifName + _SIZEOF_ADDR_IFREQ(*ifName));
#else
      ifName++;
#endif

    }
  }
  return TRUE;
}

#ifdef P_VXWORKS

int h_errno;

struct hostent * Vx_gethostbyname(char *name, struct hostent *hp)
{
  u_long addr;
  static char staticgethostname[100];

  hp->h_aliases = NULL;
  hp->h_addr_list[1] = NULL;
  if ((int)(addr = inet_addr(name)) != ERROR) {
    memcpy(staticgethostname, &addr, sizeof(addr));
    hp->h_addr_list[0] = staticgethostname;
    h_errno = SUCCESS;
    return hp;
  }
  memcpy(staticgethostname, &addr, sizeof (addr));
  hp->h_addr_list[0] = staticgethostname;
  h_errno = SUCCESS;
  return hp;
}

struct hostent * Vx_gethostbyaddr(char *name, struct hostent *hp)
{
  u_long addr;
  static char staticgethostaddr[100];

  hp->h_aliases = NULL;
  hp->h_addr_list = NULL;

  if ((int)(addr = inet_addr(name)) != ERROR) {
    char ipStorage[INET_ADDR_LEN];
    inet_ntoa_b(*(struct in_addr*)&addr, ipStorage);
    sprintf(staticgethostaddr,"%s",ipStorage);
    hp->h_name = staticgethostaddr;
    h_errno = SUCCESS;
  }
  else
  {
    printf ("_gethostbyaddr: not able to get %s\n",name);
    h_errno = NOTFOUND;
  }
  return hp;
}

#endif // P_VXWORKS


#include "../common/pethsock.cxx"

//////////////////////////////////////////////////////////////////////////////
// PUDPSocket

void PUDPSocket::EnableGQoS()
{
}

BOOL PUDPSocket::SupportQoS(const PIPSocket::Address & )
{
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

