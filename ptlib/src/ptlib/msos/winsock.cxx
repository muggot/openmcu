/*
 * winsock.cxx
 *
 * WINSOCK implementation of Berkley sockets.
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
 * $Log: winsock.cxx,v $
 * Revision 1.76  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.75  2007/09/30 17:34:40  dsandras
 * Killed GCC 4.2 warnings.
 *
 * Revision 1.74  2006/10/03 06:28:26  csoutheren
 * Ensure ios bits are reset when a PChannel is closed
 *
 * Revision 1.73  2006/01/31 03:38:27  csoutheren
 * Refixed fix for compiler warning
 *
 * Revision 1.72  2006/01/31 03:23:17  csoutheren
 * Fixed compile warning on MSVC 6
 *
 * Revision 1.71  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.70  2005/11/21 11:49:36  shorne
 * Changed disableQos to disableGQoS to better reflect what it does
 *
 * Revision 1.69  2005/09/23 15:30:46  dominance
 * more progress to make mingw compile nicely. Thanks goes to Julien Puydt for pointing out to me how to do it properly. ;)
 *
 * Revision 1.68  2005/09/18 13:01:44  dominance
 * fixed pragma warnings when building with gcc.
 *
 * Revision 1.67  2005/08/08 06:59:39  rjongbloed
 * Fixed compiler warning
 *
 * Revision 1.66  2005/07/13 12:08:09  csoutheren
 * Fixed QoS patches to be more consistent with PWLib style and to allow Unix compatibility
 *
 * Revision 1.65  2005/07/13 11:48:55  csoutheren
 * Backported QOS changes from isvo branch
 *
 * Revision 1.64  2004/10/23 10:45:32  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.63  2004/05/06 11:28:30  rjongbloed
 * Changed P_fd_set to use malloc/free isntead of new/delete due to pedantry about [].
 *
 * Revision 1.62  2004/04/27 09:53:27  rjongbloed
 *  Fixed ability to break of a PSocket::Select call under linux when a socket
 *    is closed by another thread.
 *
 * Revision 1.61  2004/04/03 08:22:22  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.60  2003/11/12 04:40:58  csoutheren
 * Fixed linking problem on systems without QoS or IPV6
 *
 * Revision 1.59  2003/11/10 00:21:38  dereksmithies
 * Stop compiler warnings (unused formal parameters) when P_HAS_QOS is on
 *
 * Revision 1.58  2003/10/30 11:33:59  rjongbloed
 * Added automatic inclusion of Winsock2 library.
 *
 * Revision 1.57  2003/10/28 23:36:22  csoutheren
 * Changed to use ws2_32.lib or wsock32.lib depending on use of QoS
 *
 * Revision 1.56  2003/10/27 08:01:52  csoutheren
 * Removed use of GetAddressByName when using Winsock2
 *
 * Revision 1.55  2003/10/27 03:29:11  csoutheren
 * Added support for QoS
 *    Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.54  2003/09/17 05:45:10  csoutheren
 * Removed recursive includes
 *
 * Revision 1.53  2002/10/29 08:00:16  robertj
 * Changed in_addr6 to more universally used in6_addr.
 *
 * Revision 1.52  2002/10/19 06:12:20  robertj
 * Moved P_fd_set::Zero() from platform independent to platform dependent
 *   code as Win32 implementation is completely different from Unix.
 *
 * Revision 1.51  2002/10/17 07:17:43  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.50  2002/10/08 12:41:52  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.49  2002/05/23 09:07:41  robertj
 * Further adjustments to compensate for Winsock weirdness on some platforms.
 *
 * Revision 1.48  2002/05/23 01:54:35  robertj
 * Worked around WinSock bug where getsockopt() does not work immediately
 *   after the select() function returns an exception.
 *
 * Revision 1.47  2002/05/22 07:22:17  robertj
 * Fixed bug in waiting for connect with a timeout not checking for errors via
 *   the except fdset in the select() call. Would give timeout for all errors.
 *
 * Revision 1.46  2002/04/12 01:42:41  robertj
 * Changed return value on os_connect() and os_accept() to make sure
 *   get the correct error codes propagated up under unix.
 *
 * Revision 1.45  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.44  2001/09/06 02:30:31  robertj
 * Fixed mismatched declarations, thanks Vjacheslav Andrejev
 *
 * Revision 1.43  2001/03/20 06:57:14  robertj
 * os_accept() function changed due to unix changes re unblocking threads.
 *
 * Revision 1.42  2001/01/24 06:46:45  yurik
 * Windows CE port-related changes
 *
 * Revision 1.41  1998/11/30 04:50:19  robertj
 * New directory structure
 *
 * Revision 1.40  1998/11/14 06:31:15  robertj
 * Changed semantics of os_sendto to return TRUE if ANY bytes are sent.
 *
 * Revision 1.39  1998/09/24 03:31:02  robertj
 * Added open software license.
 *
 * Revision 1.38  1998/08/28 14:09:45  robertj
 * Fixed bug in Write() that caused endlesss loops, introduced in previous version.
 *
 * Revision 1.37  1998/08/21 05:27:31  robertj
 * Fixed bug where write streams out to non-stream socket.
 *
 * Revision 1.36  1998/08/06 00:55:21  robertj
 * Fixed conversion of text to IPX address, was swapping nibbles.
 *
 * Revision 1.35  1998/05/08 11:52:03  robertj
 * Added workaround for winsock bug where getpeername() doesn't work immediately after connect().
 *
 * Revision 1.34  1998/05/07 05:21:04  robertj
 * Fixed DNS lookup so only works around bug in old Win95 and not OSR2
 *
 * Revision 1.33  1998/01/26 01:00:06  robertj
 * Added timeout to os_connect().
 * Fixed problems with NT version of IsLocalHost().
 *
 * Revision 1.32  1997/12/18 05:05:27  robertj
 * Moved IsLocalHost() to platform dependent code.
 *
 * Revision 1.31  1997/12/11 10:41:55  robertj
 * Added DWORD operator for IP addresses.
 *
 * Revision 1.30  1997/01/03 04:37:11  robertj
 * Fixed '95 problem with send timeouts.
 *
 * Revision 1.29  1996/12/05 11:51:50  craigs
 * Fixed Win95 recvfrom timeout problem
 *
 * Revision 1.28  1996/11/10 21:04:56  robertj
 * Fixed bug in not flushing stream on close of socket.
 *
 * Revision 1.27  1996/10/31 12:39:30  robertj
 * Fixed bug in byte order of port numbers in IPX protocol.
 *
 * Revision 1.26  1996/10/26 01:43:18  robertj
 * Removed translation of IP address to host order DWORD. Is ALWAYS net order.
 *
 * Revision 1.25  1996/10/08 13:03:09  robertj
 * More IPX support.
 *
 * Revision 1.24  1996/09/14 13:09:47  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.23  1996/08/08 10:06:07  robertj
 * Fixed incorrect value in write, causes incorrect output if send is split.
 *
 * Revision 1.22  1996/07/27 04:03:29  robertj
 * Created static version of ConvertOSError().
 *
 * Revision 1.21  1996/06/01 04:19:34  robertj
 * Added flush to PSocket destructor as needs to use Write() at that level.
 *
 * Revision 1.20  1996/05/15 10:23:08  robertj
 * Changed millisecond access functions to get 64 bit integer.
 * Added timeout to accept function.
 * Added ICMP protocol socket, getting common ancestor to UDP.
 *
 * Revision 1.19  1996/04/29 12:22:26  robertj
 * Fixed detection of infinite timeout.
 *
 * Revision 1.18  1996/04/17 12:09:52  robertj
 * Fixed bug in detecting infinte timeout.
 *
 * Revision 1.17  1996/04/12 09:45:06  robertj
 * Rewrite of PSocket::Read() to avoid "Connection Reset" errors caused by SO_RCVTIMEO
 *
 * Revision 1.17  1996/04/10 12:15:11  robertj
 * Rewrite of PSocket::Read() to avoid "Connection Reset" errors caused by SO_RCVTIMEO.
 *
 * Revision 1.16  1996/04/05 01:42:28  robertj
 * Assured PSocket::Write always writes the number of bytes specified.
 *
 * Revision 1.15  1996/03/31 09:11:06  robertj
 * Fixed major performance problem in timeout read/write to sockets.
 *
 * Revision 1.14  1996/03/10 13:16:25  robertj
 * Fixed ioctl of closed socket.
 *
 * Revision 1.13  1996/03/04 12:41:02  robertj
 * Fixed bug in leaving socket in non-blocking mode.
 * Changed _Close to os_close to be consistent.
 *
 * Revision 1.12  1996/02/25 11:23:40  robertj
 * Fixed bug in Read for when a timeout occurs on select, not returning error code.
 *
 * Revision 1.11  1996/02/25 03:13:12  robertj
 * Moved some socket functions to platform dependent code.
 *
 * Revision 1.10  1996/02/19 13:52:39  robertj
 * Added SO_LINGER option to socket to stop data loss on close.
 * Fixed error reporting for winsock classes.
 *
 * Revision 1.9  1996/02/15 14:53:36  robertj
 * Added Select() function to PSocket.
 *
 * Revision 1.8  1996/01/23 13:25:48  robertj
 * Moved Accept from platform independent code.
 *
 * Revision 1.7  1996/01/02 12:57:17  robertj
 * Unix compatibility.
 *
 * Revision 1.6  1995/12/10 12:06:00  robertj
 * Numerous fixes for sockets.
 *
 * Revision 1.5  1995/06/17 00:59:49  robertj
 * Fixed bug with stream being flushed on read/write.
 *
 * Revision 1.4  1995/06/04 12:49:51  robertj
 * Fixed bugs in socket read and write function return status.
 * Fixed bug in socket close setting object state to "closed".
 *
 * Revision 1.3  1995/03/12 05:00:10  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.2  1995/01/03  09:43:27  robertj
 * Moved out of band stuff to common.
 *
 * Revision 1.1  1994/10/30  12:06:56  robertj
 * Initial revision
 */

#include <ptlib.h>
#include <ptlib/sockets.h>

#include <svcguid.h>

#ifndef _WIN32_WCE
  #include <nspapi.h>
  #include <wsipx.h>

  #ifdef _MSC_VER
    #include <wsnwlink.h>

    #if defined(P_WINSOCK2_LIBRARY)
      #pragma comment(lib, P_WINSOCK2_LIBRARY)
    #else
      #pragma comment(lib, "wsock32.lib")
    #endif

  #else

    #define IPX_PTYPE 0x4000
    #define NS_DEFAULT 0

    #ifndef SVCID_NETWARE
    #define SVCID_NETWARE(_SapId) {(0x000B << 16)|(_SapId),0,0,{0xC0,0,0,0,0,0,0,0x46}}
    #endif /* SVCID_NETWARE */

    #define SVCID_FILE_SERVER SVCID_NETWARE(0x4)

  #endif

#endif // !_WIN32_WCE


//////////////////////////////////////////////////////////////////////////////
// PWinSock

PWinSock::PWinSock()
{
  WSADATA winsock;

#if 0 // old WinSock version check
  PAssert(WSAStartup(0x101, &winsock) == 0, POperatingSystemError);
  PAssert(LOBYTE(winsock.wVersion) == 1 &&
          HIBYTE(winsock.wVersion) == 1, POperatingSystemError);

#endif

  // ensure we support QoS
  PAssert(WSAStartup(0x0202, &winsock) == 0, POperatingSystemError);
  PAssert(LOBYTE(winsock.wVersion) >= 1 &&
          HIBYTE(winsock.wVersion) >= 1, POperatingSystemError);
}


PWinSock::~PWinSock()
{
  WSACleanup();
}


BOOL PWinSock::OpenSocket()
{
  return FALSE;
}


const char * PWinSock::GetProtocolName() const
{
  return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// P_fd_set

void P_fd_set::Construct()
{
  max_fd = UINT_MAX;
  set = (fd_set *)malloc(sizeof(fd_set));
}


void P_fd_set::Zero()
{
  if (PAssertNULL(set) != NULL)
    FD_ZERO(set);
}


//////////////////////////////////////////////////////////////////////////////
// PSocket

PSocket::~PSocket()
{
  Close();
}


BOOL PSocket::Read(void * buf, PINDEX len)
{
  flush();
  lastReadCount = 0;

  if (len == 0)
    return SetErrorValues(BadParameter, EINVAL, LastReadError);

  os_recvfrom((char *)buf, len, 0, NULL, NULL);
  return lastReadCount > 0;
}


BOOL PSocket::Write(const void * buf, PINDEX len)
{
  flush();
  return os_sendto(buf, len, 0, NULL, 0) && lastWriteCount >= len;
}


BOOL PSocket::Close()
{
  if (!IsOpen())
    return FALSE;
  flush();
  return ConvertOSError(os_close());
}


int PSocket::os_close()
{
  int err = closesocket(os_handle);
  os_handle = -1;
  clear();
  return err;
}


int PSocket::os_socket(int af, int type, int proto)
{
  return ::socket(af, type, proto);
}


BOOL PSocket::os_connect(struct sockaddr * addr, PINDEX size)
{
  if (readTimeout == PMaxTimeInterval)
    return ConvertOSError(::connect(os_handle, addr, size));

  DWORD fionbio = 1;
  if (!ConvertOSError(::ioctlsocket(os_handle, FIONBIO, &fionbio)))
    return FALSE;
  fionbio = 0;

  if (::connect(os_handle, addr, size) != SOCKET_ERROR)
    return ConvertOSError(::ioctlsocket(os_handle, FIONBIO, &fionbio));

  DWORD err = GetLastError();
  if (err != WSAEWOULDBLOCK) {
    ::ioctlsocket(os_handle, FIONBIO, &fionbio);
    SetLastError(err);
    return ConvertOSError(-1);
  }

  P_fd_set writefds = os_handle;
  P_fd_set exceptfds = os_handle;
  P_timeval tv;

  /* To avoid some strange behaviour on various windows platforms, do a zero
     timeout select first to pick up errors. Then do real timeout. */
  int selerr = ::select(1, NULL, writefds, exceptfds, tv);
  if (selerr == 0) {
    writefds = os_handle;
    exceptfds = os_handle;
    tv = readTimeout;
    selerr = ::select(1, NULL, writefds, exceptfds, tv);
  }

  switch (selerr) {
    case 1 :
      if (writefds.IsPresent(os_handle)) {
        // The following is to avoid a bug in Win32 sockets. The getpeername() function doesn't
        // work for some period of time after a connect, saying it is not connected yet!
        for (PINDEX failsafe = 0; failsafe < 1000; failsafe++) {
          sockaddr_in address;
          int sz = sizeof(address);
          if (::getpeername(os_handle, (struct sockaddr *)&address, &sz) == 0) {
            if (address.sin_port != 0)
              break;
          }
          ::Sleep(0);
        }

        err = 0;
      }
      else {
        // The following is to avoid a bug in Win32 sockets. The getsockopt() function
        // doesn't work for some period of time after a connect, saying no error!
        for (PINDEX failsafe = 0; failsafe < 1000; failsafe++) {
          int sz = sizeof(err);
          if (::getsockopt(os_handle, SOL_SOCKET, SO_ERROR, (char *)&err, &sz) == 0) {
            if (err != 0)
              break;
          }
          ::Sleep(0);
        }
        if (err == 0)
          err = WSAEFAULT; // Need to have something!
      }
      break;

    case 0 :
      err = WSAETIMEDOUT;
      break;

    default :
      err = GetLastError();
  }

  if (::ioctlsocket(os_handle, FIONBIO, &fionbio) == SOCKET_ERROR) {
    if (err == 0)
      err = GetLastError();
  }

  SetLastError(err);
  return ConvertOSError(err == 0 ? 0 : SOCKET_ERROR);
}


BOOL PSocket::os_accept(PSocket & listener, struct sockaddr * addr, int * size)
{
  if (listener.GetReadTimeout() != PMaxTimeInterval) {
    P_fd_set readfds = listener.GetHandle();
    P_timeval tv = listener.GetReadTimeout();
    switch (select(0, readfds, NULL, NULL, tv)) {
      case 1 :
        break;
      case 0 :
        SetLastError(WSAETIMEDOUT);
        // Then return -1
      default :
        return ConvertOSError(-1);
    }
  }
  return ConvertOSError(os_handle = ::accept(listener.GetHandle(), addr, size));
}


BOOL PSocket::os_recvfrom(void * buf,
                          PINDEX len,
                          int flags,
                          struct sockaddr * from,
                          PINDEX * fromlen)
{
  lastReadCount = 0;

  if (readTimeout != PMaxTimeInterval) {
    DWORD available;
    if (!ConvertOSError(ioctlsocket(os_handle, FIONREAD, &available), LastReadError))
      return FALSE;

    if (available == 0) {
      P_fd_set readfds = os_handle;
      P_timeval tv = readTimeout;
      int selval = ::select(0, readfds, NULL, NULL, tv);
      if (!ConvertOSError(selval, LastReadError))
        return FALSE;

      if (selval == 0)
        return SetErrorValues(Timeout, EAGAIN, LastReadError);

      if (!ConvertOSError(ioctlsocket(os_handle, FIONREAD, &available), LastReadError))
        return FALSE;
    }

    if (available > 0 && len > (PINDEX)available)
      len = available;
  }

  int recvResult = ::recvfrom(os_handle, (char *)buf, len, flags, from, fromlen);
  if (!ConvertOSError(recvResult, LastReadError))
    return FALSE;

  lastReadCount = recvResult;
  return TRUE;
}


BOOL PSocket::os_sendto(const void * buf,
                        PINDEX len,
                        int flags,
                        struct sockaddr * to,
                        PINDEX tolen)
{
  lastWriteCount = 0;

  if (writeTimeout != PMaxTimeInterval) {
    P_fd_set writefds = os_handle;
    P_timeval tv = writeTimeout;
    int selval = ::select(0, NULL, writefds, NULL, tv);
    if (selval < 0)
      return FALSE;

    if (selval == 0) {
#ifndef _WIN32_WCE
      errno = EAGAIN;
#else
      SetLastError(EAGAIN);
#endif
      return FALSE;
    }
  }

  int sendResult = ::sendto(os_handle, (const char *)buf, len, flags, to, tolen);
  if (!ConvertOSError(sendResult, LastWriteError))
    return FALSE;

  if (sendResult == 0)
    return FALSE;

  lastWriteCount = sendResult;
  return TRUE;
}


PChannel::Errors PSocket::Select(SelectList & read,
                                 SelectList & write,
                                 SelectList & except,
                                 const PTimeInterval & timeout)
{
  PINDEX i;

  P_fd_set readfds;
  for (i = 0; i < read.GetSize(); i++) {
    if (!read[i].IsOpen())
      return NotOpen;
    readfds += read[i].GetHandle();
  }

  P_fd_set writefds;
  for (i = 0; i < write.GetSize(); i++) {
    if (!write[i].IsOpen())
      return NotOpen;
    writefds += write[i].GetHandle();
  }

  P_fd_set exceptfds;
  for (i = 0; i < except.GetSize(); i++) {
    if (!except[i].IsOpen())
      return NotOpen;
    exceptfds += except[i].GetHandle();
  }

  P_timeval tval = timeout;
  int retval = select(INT_MAX, readfds, writefds, exceptfds, tval);

  Errors lastError;
  int osError;
  if (!ConvertOSError(retval, lastError, osError))
    return lastError;

  if (retval > 0) {
    for (i = 0; i < read.GetSize(); i++) {
      int h = read[i].GetHandle();
      if (h < 0)
        return Interrupted;
      if (!readfds.IsPresent(h))
        read.RemoveAt(i--);
    }
    for (i = 0; i < write.GetSize(); i++) {
      int h = write[i].GetHandle();
      if (h < 0)
        return Interrupted;
      if (!writefds.IsPresent(h))
        write.RemoveAt(i--);
    }
    for (i = 0; i < except.GetSize(); i++) {
      int h = except[i].GetHandle();
      if (h < 0)
        return Interrupted;
      if (!exceptfds.IsPresent(h))
        except.RemoveAt(i--);
    }
  }
  else {
    read.RemoveAll();
    write.RemoveAll();
    except.RemoveAll();
  }

  return NoError;
}


BOOL PSocket::ConvertOSError(int status, ErrorGroup group)
{
  Errors lastError;
  int osError;
  BOOL ok = ConvertOSError(status, lastError, osError);
  SetErrorValues(lastError, osError, group);
  return ok;
}


BOOL PSocket::ConvertOSError(int status, Errors & lastError, int & osError)
{
  if (status >= 0) {
    lastError = NoError;
    osError = 0;
    return TRUE;
  }

#ifdef _WIN32
  SetLastError(WSAGetLastError());
  return PChannel::ConvertOSError(-2, lastError, osError);
#else
  osError = WSAGetLastError();
  switch (osError) {
    case 0 :
      lastError = NoError;
      return TRUE;
    case WSAEWOULDBLOCK :
      lastError = Timeout;
      break;
    default :
      osError |= PWIN32ErrorFlag;
      lastError = Miscellaneous;
  }
  return FALSE;
#endif
}


//////////////////////////////////////////////////////////////////////////////
// PIPSocket::Address

PIPSocket::Address::Address(BYTE b1, BYTE b2, BYTE b3, BYTE b4)
{
  version = 4;
  v.four.S_un.S_un_b.s_b1 = b1;
  v.four.S_un.S_un_b.s_b2 = b2;
  v.four.S_un.S_un_b.s_b3 = b3;
  v.four.S_un.S_un_b.s_b4 = b4;
}


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
    v.four.S_un.S_addr = dw;
  }
  return *this;
}


PIPSocket::Address::operator DWORD() const
{
  return version != 4 ? 0 : v.four.S_un.S_addr;
}


BYTE PIPSocket::Address::Byte1() const
{
  return v.four.S_un.S_un_b.s_b1;
}


BYTE PIPSocket::Address::Byte2() const
{
  return v.four.S_un.S_un_b.s_b2;
}


BYTE PIPSocket::Address::Byte3() const
{
  return v.four.S_un.S_un_b.s_b3;
}


BYTE PIPSocket::Address::Byte4() const
{
  return v.four.S_un.S_un_b.s_b4;
}


//////////////////////////////////////////////////////////////////////////////
// PIPSocket

BOOL P_IsOldWin95()
{
  static int state = -1;
  if (state < 0) {
    state = 1;
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(info);
    if (GetVersionEx(&info)) {
      state = 0;
      if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && info.dwBuildNumber < 1000)
        state = 1;
    }
  }
  return state != 0;
}


BOOL PIPSocket::IsLocalHost(const PString & hostname)
{
  if (hostname.IsEmpty())
    return TRUE;

  if (hostname *= "localhost")
    return TRUE;

  // lookup the host address using inet_addr, assuming it is a "." address
  PIPSocket::Address addr = hostname;
  if (addr.IsLoopback())  // Is 127.0.0.1 or ::1
    return TRUE;

  if (addr == 0) {
    if (!GetHostAddress(hostname, addr))
      return FALSE;
  }

  // Seb: Should check that it's really IPv4 aware.
  struct hostent * host_info = ::gethostbyname(GetHostName());

  if (P_IsOldWin95())
    return addr == *(struct in_addr *)host_info->h_addr_list[0];

  for (PINDEX i = 0; host_info->h_addr_list[i] != NULL; i++) {
#if P_HAS_IPV6
    if (host_info->h_length == 16) {
      if (addr == *(struct in6_addr *)host_info->h_addr_list[i])
        return TRUE;
    }
    else
#endif
    if (addr == *(struct in_addr *)host_info->h_addr_list[i])
      return TRUE;
  }
  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// PUDPSocket

BOOL PUDPSocket::disableGQoS = TRUE;

void PUDPSocket::EnableGQoS()
{
  disableGQoS = FALSE;
}

#if P_HAS_QOS
BOOL PUDPSocket::SupportQoS(const PIPSocket::Address & address)
{
  if (disableGQoS)
    return FALSE;

  if (!address.IsValid())
    return FALSE;

  // Check to See if OS supportive
    OSVERSIONINFO versInfo;
    ZeroMemory(&versInfo,sizeof(OSVERSIONINFO));
    versInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!(GetVersionEx(&versInfo)))
        return FALSE;
    else
    {
        if (versInfo.dwMajorVersion < 5)
            return FALSE;  // Not Supported in Windows

        if (versInfo.dwMajorVersion == 5 &&
            versInfo.dwMinorVersion == 0)
            return FALSE;         //Windows 2000 does not always support QOS_DESTADDR
    }

  // Need to put in a check to see if the NIC has 802.1p packet priority support 
  // This Requires access to the NIC driver and requires Windows DDK. To Be Done Sometime...
  
  // Get the name of the required NIC to check whether it supports 802.1p
  PString NICname =  PIPSocket::GetInterface(address);

  // For Now Assume it can.
  return TRUE;
}

#else

BOOL PUDPSocket::SupportQoS(const PIPSocket::Address &)
{
  return FALSE;
}
#endif  // P_HAS_QOS


#if P_HAS_QOS

#ifndef _WIN32_WCE

PWinQoS::~PWinQoS()
{
    delete sa;
}

PWinQoS::PWinQoS(PQoS & pqos, struct sockaddr * to, char * inBuf, DWORD & bufLen)
{
  QOS * qos = (QOS *)inBuf;
    
  if (pqos.GetTokenRate() == QOS_NOT_SPECIFIED)
    qos->SendingFlowspec.ServiceType = SERVICETYPE_BESTEFFORT;
  else
    qos->SendingFlowspec.ServiceType = pqos.GetServiceType();
    
  qos->SendingFlowspec.TokenRate = pqos.GetTokenRate();
  qos->SendingFlowspec.TokenBucketSize = pqos.GetTokenBucketSize();
  qos->SendingFlowspec.PeakBandwidth = pqos.GetPeakBandwidth();
  qos->SendingFlowspec.Latency = QOS_NOT_SPECIFIED;
  qos->SendingFlowspec.DelayVariation = QOS_NOT_SPECIFIED;
  qos->SendingFlowspec.MaxSduSize = QOS_NOT_SPECIFIED;
  qos->SendingFlowspec.MinimumPolicedSize = QOS_NOT_SPECIFIED;

  qos->ReceivingFlowspec.ServiceType = SERVICETYPE_BESTEFFORT|SERVICE_NO_QOS_SIGNALING;
  qos->ReceivingFlowspec.TokenRate = QOS_NOT_SPECIFIED;
  qos->ReceivingFlowspec.TokenBucketSize = QOS_NOT_SPECIFIED;
  qos->ReceivingFlowspec.PeakBandwidth = QOS_NOT_SPECIFIED;
  qos->ReceivingFlowspec.Latency = QOS_NOT_SPECIFIED;
  qos->ReceivingFlowspec.DelayVariation = QOS_NOT_SPECIFIED;
  qos->ReceivingFlowspec.MaxSduSize = QOS_NOT_SPECIFIED;
  qos->ReceivingFlowspec.MinimumPolicedSize = QOS_NOT_SPECIFIED;

  sa = new sockaddr;
  *sa = *to;

  QOS_DESTADDR qosdestaddr;
  qosdestaddr.ObjectHdr.ObjectType = QOS_OBJECT_DESTADDR;
  qosdestaddr.ObjectHdr.ObjectLength = sizeof(qosdestaddr);
  qosdestaddr.SocketAddress = sa;
  qosdestaddr.SocketAddressLength = sizeof(*sa);

  qos->ProviderSpecific.len = sizeof(qosdestaddr);
  qos->ProviderSpecific.buf = inBuf + sizeof(*qos);

  memcpy(inBuf+sizeof(*qos),&qosdestaddr,sizeof(qosdestaddr));
  bufLen = sizeof(*qos)+sizeof(qosdestaddr);
}

#endif // _WIN32_WCE

#endif // P_HAS_QOS

#ifndef _WIN32_WCE
//////////////////////////////////////////////////////////////////////////////
// PIPXSocket

PIPXSocket::Address::Address()
{
  memset(this, 0, sizeof(*this));
}


PIPXSocket::Address::Address(const Address & addr)
{
  memcpy(this, &addr, sizeof(*this));
}


PIPXSocket::Address::Address(const PString & str)
{
  PINDEX colon = str.Find(':');
  if (colon == P_MAX_INDEX)
    colon = 0;
  else {
    DWORD netnum = 0;
    for (PINDEX i = 0; i < colon; i++) {
      int c = str[i];
      if (isdigit(c))
        netnum = (netnum << 4) + c - '0';
      else if (isxdigit(c))
        netnum = (netnum << 4) + toupper(c) - 'A' + 10;
      else {
        memset(this, 0, sizeof(*this));
        return;
      }
    }
    network.dw = ntohl(netnum);
  }

  memset(node, 0, sizeof(node));

  int shift = 0;
  PINDEX byte = 5;
  PINDEX pos = str.GetLength();
  while (--pos > colon) {
    int c = str[pos];
    if (c != '-') {
      if (isdigit(c))
        node[byte] |= (c - '0') << shift;
      else if (isxdigit(c))
        node[byte] |= (toupper(c) - 'A' + 10) << shift;
      else {
        memset(this, 0, sizeof(*this));
        return;
      }
      if (shift == 0)
        shift = 4;
      else {
        shift = 0;
        byte--;
      }
    }
  }
}


PIPXSocket::Address::Address(DWORD netNum, const char * nodeNum)
{
  network.dw = netNum;
  memcpy(node, nodeNum, sizeof(node));
}


PIPXSocket::Address & PIPXSocket::Address::operator=(const Address & addr)
{
  memcpy(this, &addr, sizeof(*this));
  return *this;
}


PIPXSocket::Address::operator PString() const
{
  return psprintf("%02X%02X%02X%02X:%02X%02X%02X%02X%02X%02X",
                  network.b.b1, network.b.b2, network.b.b3, network.b.b4,
                  node[0], node[1], node[2], node[3], node[4], node[5]);
}


BOOL PIPXSocket::Address::IsValid() const
{
  static Address empty;
  return memcmp(this, &empty, sizeof(empty)) != 0;
}


PIPXSocket::PIPXSocket(WORD newPort)
{
  SetPort(newPort);
}


PString PIPXSocket::GetName() const
{
  Address addr;
  if (((PIPXSocket*)this)->GetPeerAddress(addr))
    return addr;
  else
    return PString();
}


BOOL PIPXSocket::OpenSocket()
{
  return ConvertOSError(os_handle = os_socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX));
}


const char * PIPXSocket::GetProtocolName() const
{
  return "ipx";
}


BOOL PIPXSocket::SetPacketType(int type)
{
  return ConvertOSError(::setsockopt(os_handle,
                           NSPROTO_IPX, IPX_PTYPE, (char *)&type, sizeof(type)));
}


int PIPXSocket::GetPacketType()
{
  int value;
  int valSize = sizeof(value);
  if (ConvertOSError(::getsockopt(os_handle,
                                NSPROTO_IPX, IPX_PTYPE, (char *)&value, &valSize)))
    return value;
  return -1;
}


PString PIPXSocket::GetHostName(const Address & addr)
{
  return addr;
}


BOOL PIPXSocket::GetHostAddress(Address &)
{
  return FALSE;
}


static void AssignAddress(sockaddr_ipx & sip, const PIPXSocket::Address & addr)
{
  memcpy(sip.sa_netnum, &addr.network, sizeof(sip.sa_netnum));
  memcpy(sip.sa_nodenum, addr.node, sizeof(sip.sa_nodenum));
}


static void AssignAddress(PIPXSocket::Address & addr, const sockaddr_ipx & sip)
{
  memcpy(&addr.network, sip.sa_netnum, sizeof(addr.network));
  memcpy(addr.node, sip.sa_nodenum, sizeof(addr.node));
}


#ifdef P_HAS_QOS
BOOL PIPXSocket::GetHostAddress(const PString & /*hostname*/, Address & /*addr*/)
{
  return FALSE;
}
#else
BOOL PIPXSocket::GetHostAddress(const PString & hostname, Address & addr)
{
  addr = hostname;
  if (addr.IsValid())
    return TRUE;

  static GUID netware_file_server = SVCID_FILE_SERVER;
  CSADDR_INFO addr_info[10];
  DWORD buffer_length = sizeof(addr_info);
  int num = GetAddressByName(NS_DEFAULT,
                             &netware_file_server,
                             (LPTSTR)(const char *)hostname,
                             NULL,
                             0,
                             NULL,
                             addr_info,
                             &buffer_length,
                             NULL,
                             NULL
                            );
  if (num <= 0)
    return FALSE;

  AssignAddress(addr, *(sockaddr_ipx *)addr_info[0].RemoteAddr.lpSockaddr);
  return TRUE;
}
#endif



BOOL PIPXSocket::GetLocalAddress(Address & addr)
{
  sockaddr_ipx sip;
  int size = sizeof(sip);
  if (!ConvertOSError(::getsockname(os_handle, (struct sockaddr *)&sip, &size)))
    return FALSE;

  AssignAddress(addr, sip);
  return TRUE;
}


BOOL PIPXSocket::GetLocalAddress(Address & addr, WORD & portNum)
{
  sockaddr_ipx sip;
  int size = sizeof(sip);
  if (!ConvertOSError(::getsockname(os_handle, (struct sockaddr *)&sip, &size)))
    return FALSE;

  AssignAddress(addr, sip);
  portNum = Net2Host(sip.sa_socket);
  return TRUE;
}


BOOL PIPXSocket::GetPeerAddress(Address & addr)
{
  sockaddr_ipx sip;
  int size = sizeof(sip);
  if (!ConvertOSError(::getpeername(os_handle, (struct sockaddr *)&sip, &size)))
    return FALSE;

  AssignAddress(addr, sip);
  return TRUE;
}


BOOL PIPXSocket::GetPeerAddress(Address & addr, WORD & portNum)
{
  sockaddr_ipx sip;
  int size = sizeof(sip);
  if (!ConvertOSError(::getpeername(os_handle, (struct sockaddr *)&sip, &size)))
    return FALSE;

  AssignAddress(addr, sip);
  portNum = Net2Host(sip.sa_socket);
  return TRUE;
}


BOOL PIPXSocket::Connect(const PString & host)
{
  Address addr;
  if (GetHostAddress(host, addr))
    return Connect(addr);
  return FALSE;
}


BOOL PIPXSocket::Connect(const Address & addr)
{
  // close the port if it is already open
  if (IsOpen())
    Close();

  // make sure we have a port
  PAssert(port != 0, "Cannot connect socket without setting port");

  // attempt to create a socket
  if (!OpenSocket())
    return FALSE;

  // attempt to lookup the host name
  sockaddr_ipx sip;
  memset(&sip, 0, sizeof(sip));
  sip.sa_family = AF_IPX;
  AssignAddress(sip, addr);
  sip.sa_socket  = Host2Net(port);  // set the port
  if (os_connect((struct sockaddr *)&sip, sizeof(sip)))
    return TRUE;

  os_close();
  return FALSE;
}


BOOL PIPXSocket::Listen(unsigned, WORD newPort, Reusability reuse)
{
  // make sure we have a port
  if (newPort != 0)
    port = newPort;

  // close the port if it is already open
  if (!IsOpen()) {
    // attempt to create a socket
    if (!OpenSocket())
      return FALSE;
  }

  // attempt to listen
  if (SetOption(SO_REUSEADDR, reuse == CanReuseAddress ? 1 : 0)) {
    // attempt to listen
    sockaddr_ipx sip;
    memset(&sip, 0, sizeof(sip));
    sip.sa_family = AF_IPX;
    sip.sa_socket = Host2Net(port);       // set the port

    if (ConvertOSError(::bind(os_handle, (struct sockaddr*)&sip, sizeof(sip)))) {
      int size = sizeof(sip);
      if (ConvertOSError(::getsockname(os_handle, (struct sockaddr*)&sip, &size))) {
        port = Net2Host(sip.sa_socket);
        return TRUE;
      }
    }
  }

  os_close();
  return FALSE;
}


BOOL PIPXSocket::ReadFrom(void * buf, PINDEX len, Address & addr, WORD & port)
{
  lastReadCount = 0;

  sockaddr_ipx sip;
  int addrLen = sizeof(sip);
  if (os_recvfrom(buf, len, 0, (struct sockaddr *)&sip, &addrLen)) {
    AssignAddress(addr, sip);
    port = Net2Host(sip.sa_socket);
  }

  return lastReadCount > 0;
}


BOOL PIPXSocket::WriteTo(const void * buf, PINDEX len, const Address & addr, WORD port)
{
  lastWriteCount = 0;

  sockaddr_ipx sip;
  sip.sa_family = AF_IPX;
  AssignAddress(sip, addr);
  sip.sa_socket = Host2Net(port);
  return os_sendto(buf, len, 0, (struct sockaddr *)&sip, sizeof(sip));
}


//////////////////////////////////////////////////////////////////////////////
// PSPXSocket

PSPXSocket::PSPXSocket(WORD port)
  : PIPXSocket(port)
{
}


BOOL PSPXSocket::OpenSocket()
{
  return ConvertOSError(os_handle = os_socket(AF_IPX, SOCK_STREAM, NSPROTO_SPX));
}


const char * PSPXSocket::GetProtocolName() const
{
  return "spx";
}


BOOL PSPXSocket::Listen(unsigned queueSize, WORD newPort, Reusability reuse)
{
  if (PIPXSocket::Listen(queueSize, newPort, reuse) &&
      ConvertOSError(::listen(os_handle, queueSize)))
    return TRUE;

  os_close();
  return FALSE;
}


BOOL PSPXSocket::Accept(PSocket & socket)
{
  PAssert(PIsDescendant(&socket, PIPXSocket), "Invalid listener socket");

  sockaddr_ipx sip;
  sip.sa_family = AF_IPX;
  int size = sizeof(sip);
  if (!os_accept(socket, (struct sockaddr *)&sip, &size))
    return FALSE;

  port = ((PIPXSocket &)socket).GetPort();
  return TRUE;
}

#endif
// End Of File ///////////////////////////////////////////////////////////////
