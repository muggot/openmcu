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
 * Revision 1.2  2002/04/12 01:42:41  robertj
 * Changed return value on os_connect() and os_accept() to make sure
 *   get the correct error codes propagated up under unix.
 *
 * Revision 1.1  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
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

#ifdef __GNUC__
#pragma implementation "sockets.h"
#pragma implementation "socket.h"
#pragma implementation "ipsock.h"
#pragma implementation "udpsock.h"
#pragma implementation "tcpsock.h"
#pragma implementation "ipdsock.h"
#pragma implementation "ethsock.h"
#endif

#include <ptlib.h>
#include <ptlib/sockets.h>


// Appears to have disappeared (I hope - statics impossible in Nucleus!)
//extern PSemaphore PX_iostreamMutex;

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

#ifdef __BEOS__
#ifndef BE_THREADS
  // abort any I/O block using this os_handle
  PProcess::Current().PXAbortIOBlock(os_handle);
#endif

  int retval = ::closesocket(os_handle);
  os_handle = -1;
  return retval;
#else
  return PXClose();
#endif
}

int PSocket::os_socket(int af, int type, int protocol)
{
  // attempt to create a socket
  int handle;
  if ((handle = ::socket(af, type, protocol)) >= 0) {

    // make the socket non-blocking and close on exec
#ifndef __BEOS__
#ifndef P_PTHREADS
    DWORD cmd = 1;
#endif
#else
    int cmd = -1;
#endif

    if (
#ifndef __BEOS__
#ifndef P_PTHREADS
        !ConvertOSError(::ioctl(handle, FIONBIO, &cmd)) ||
#endif
        !ConvertOSError(::fcntl(handle, F_SETFD, 1))) {
      ::shutdown(handle, 2);
#else
	!ConvertOSError(::setsockopt(handle, SOL_SOCKET, SO_NONBLOCK, &cmd, sizeof(int)))) {
      ::closesocket(handle);
#endif
      return -1;
    }
//PError << "socket " << handle << " created" << endl;
  }
  return handle;
}

BOOL PSocket::os_connect(struct sockaddr * addr, PINDEX size)
{
  int val = ::connect(os_handle, addr, size);
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


BOOL PSocket::os_accept(int sock, struct sockaddr * addr, PINDEX * size,
                       const PTimeInterval & timeout)
{
  if (!listener.PXSetIOBlock(PXAcceptBlock, listener.GetReadTimeout()))
    return SetErrorValues(listener.GetErrorCode(), listener.GetErrorNumber());

  return ConvertOSError(SetNonBlocking(::accept(listener.GetHandle(), addr, (socklen_t *)size)));
}


#ifdef __NUCLEUS_PLUS__
#define P_PTHREADS
#endif
#ifndef P_PTHREADS

int PSocket::os_select(int maxHandle,
                   fd_set & readBits,
                   fd_set & writeBits,
                   fd_set & exceptionBits,
          const PIntArray & osHandles,
      const PTimeInterval & timeout)
{
  struct timeval * tptr = NULL;

  int stat = PThread::Current()->PXBlockOnIO(maxHandle,
                                         readBits,
                                         writeBits,
                                         exceptionBits,
                                         timeout,
					 osHandles);
  if (stat <= 0)
    return stat;

  struct timeval tout = {0, 0};
  tptr = &tout;

  return ::select(maxHandle, &readBits, &writeBits, &exceptionBits, tptr);
}
                     
#else

int PSocket::os_select(int maxHandle,
                   fd_set & readBits,
                   fd_set & writeBits,
                   fd_set & exceptionBits,
          const PIntArray & ,
      const PTimeInterval & timeout)
{
  struct timeval * tptr = NULL;
  struct timeval   timeout_val;
  if (timeout != PMaxTimeInterval) {
    if (timeout.GetMilliSeconds() < 1000L*60L*60L*24L) {
      timeout_val.tv_usec = (timeout.GetMilliSeconds() % 1000) * 1000;
      timeout_val.tv_sec  = timeout.GetSeconds();
      tptr                = &timeout_val;
    }
  }

  do {
    int result = ::select(maxHandle, &readBits, &writeBits, &exceptionBits, tptr);
    if (result >= 0)
      return result;
  } while (errno == EINTR);
  return -1;
}

#endif
#ifdef __NUCLEUS_PLUS__
#undef P_PTHREADS
#endif


PIPSocket::Address::Address(DWORD dw)
{
  s_addr = dw;
}


PIPSocket::Address & PIPSocket::Address::operator=(DWORD dw)
{
  s_addr = dw;
  return *this;
}


PIPSocket::Address::operator DWORD() const
{
  return (DWORD)s_addr;
}

BYTE PIPSocket::Address::Byte1() const
{
  return *(((BYTE *)&s_addr)+0);
}

BYTE PIPSocket::Address::Byte2() const
{
  return *(((BYTE *)&s_addr)+1);
}

BYTE PIPSocket::Address::Byte3() const
{
  return *(((BYTE *)&s_addr)+2);
}

BYTE PIPSocket::Address::Byte4() const
{
  return *(((BYTE *)&s_addr)+3);
}

PIPSocket::Address::Address(BYTE b1, BYTE b2, BYTE b3, BYTE b4)
{
  BYTE * p = (BYTE *)&s_addr;
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
  if (addr == 16777343)  // Is 127.0.0.1
    return TRUE;
  if (addr == (DWORD)-1)
    return FALSE;

  if (!GetHostAddress(hostname, addr))
    return FALSE;

  PUDPSocket sock;

#ifndef __BEOS__
  // get number of interfaces
  int ifNum;
#ifdef SIOCGIFNUM
  PAssert(::ioctl(sock.GetHandle(), SIOCGIFNUM, &ifNum) >= 0, "could not do ioctl for ifNum");
#else
  ifNum = 100;
#endif

  PBYTEArray buffer;
  struct ifconf ifConf;
  ifConf.ifc_len  = ifNum * sizeof(ifreq);
  ifConf.ifc_req = (struct ifreq *)buffer.GetPointer(ifConf.ifc_len);
  
  if (ioctl(sock.GetHandle(), SIOCGIFCONF, &ifConf) >= 0) {
#ifndef SIOCGIFNUM
    ifNum = ifConf.ifc_len / sizeof(ifreq);
#endif

    int num = 0;
    for (num = 0; num < ifNum; num++) {

      ifreq * ifName = ifConf.ifc_req + num;
      struct ifreq ifReq;
      strcpy(ifReq.ifr_name, ifName->ifr_name);

      if (ioctl(sock.GetHandle(), SIOCGIFFLAGS, &ifReq) >= 0) {
        int flags = ifReq.ifr_flags;
        if (ioctl(sock.GetHandle(), SIOCGIFADDR, &ifReq) >= 0) {
          if ((flags & IFF_UP) && (addr == Address(((sockaddr_in *)&ifReq.ifr_addr)->sin_addr)))
            return TRUE;
        }
      }
    }
  }
#endif //!__BEOS__

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
  if (!PXSetIOBlock(PXReadBlock, readTimeout)) {
    lastError     = Timeout;
    return FALSE;
  }

#ifndef __BEOS__
  // attempt to read out of band data
  char buffer[32];
  int ooblen;
  while ((ooblen = ::recv(os_handle, buffer, sizeof(buffer), MSG_OOB)) > 0) 
    OnOutOfBand(buffer, ooblen);
#endif // !__BEOS__

    // attempt to read non-out of band data
  if (ConvertOSError(lastReadCount = ::recv(os_handle, (char *)buf, maxLen, 0)))
    return lastReadCount > 0;

  lastReadCount = 0;
  return FALSE;
}


int PSocket::os_recvfrom(
      void * buf,     // Data to be written as URGENT TCP data.
      PINDEX len,     // Number of bytes pointed to by <CODE>buf</CODE>.
      int    flags,
      sockaddr * addr, // Address from which the datagram was received.
      PINDEX * addrlen)
{
  if (!PXSetIOBlock(PXReadBlock, readTimeout)) {
    lastError     = Timeout;
    lastReadCount = 0;
    return 0;
  }

  // attempt to read non-out of band data
  if (ConvertOSError(lastReadCount =
        ::recvfrom(os_handle, (char *)buf, len, flags, (sockaddr *)addr, (socklen_t *)addrlen)))
    return lastReadCount > 0;

  lastReadCount = 0;
  return -1;
}


int PSocket::os_sendto(
      const void * buf,   // Data to be written as URGENT TCP data.
      PINDEX len,         // Number of bytes pointed to by <CODE>buf</CODE>.
      int flags,
      sockaddr * addr, // Address to which the datagram is sent.
      PINDEX addrlen)  
{
  lastWriteCount = 0;

  if (!IsOpen()) {
    lastError     = NotOpen;
    return 0;
  }

  // attempt to write data
  int writeResult;
  if (addr != NULL)
    writeResult = ::sendto(os_handle, (char *)buf, len, flags, (sockaddr *)addr, addrlen);
  else
    writeResult = ::send(os_handle, (char *)buf, len, flags);
  if (writeResult > 0) {
//    PThread::Yield();
    lastWriteCount = writeResult;
    return -1;
  }

  if (errno != EWOULDBLOCK)
    return ConvertOSError(-1);

  if (!PXSetIOBlock(PXWriteBlock, writeTimeout)) {
    lastError     = Timeout;
    return 0;
  }

  // attempt to write data
  if (addr != NULL)
    lastWriteCount = ::sendto(os_handle, (char *)buf, len, flags, (sockaddr *)addr, addrlen);
  else
    lastWriteCount = ::send(os_handle, (char *)buf, len, flags);
  if (ConvertOSError(lastWriteCount))
    return lastWriteCount > 0;

  return -1;
}


BOOL PSocket::Read(void * buf, PINDEX len)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

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
  else {
    lastError = NotFound;
    osError = ENOENT;
    return FALSE;
  }

#ifdef SIOCGIFHWADDR
  PUDPSocket ifsock;
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strcpy(ifr.ifr_name, interfaceName);
  if (!ConvertOSError(ioctl(ifsock.GetHandle(), SIOCGIFHWADDR, &ifr)))
    return FALSE;

  memcpy(&macAddress, ifr.ifr_hwaddr.sa_data, sizeof(macAddress));
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
#ifndef __BEOS__
  PUDPSocket ifsock;

  ifreq ifreqs[20]; // Maximum of 20 interfaces
  ifconf ifc;
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
#endif //!__BEOS__

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

#ifndef __BEOS__
  PUDPSocket ifsock;
  struct ifreq ifr;
  strstream str;
  ifr.ifr_addr.sa_family = AF_INET;
  if (idx == 0)
    strcpy(ifr.ifr_name, channelName);
  else
  {
    str<<(const char *)channelName<<':'<<(int)(idx-1)<<'\x0';
    strcpy(ifr.ifr_name, str.str());
//    sprintf(ifr.ifr_name, "%s:%u", (const char *)channelName, (int)(idx-1));
  }
  if (!ConvertOSError(ioctl(os_handle, SIOCGIFADDR, &ifr)))
    return FALSE;

  sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
  addr = sin->sin_addr;

  if (!ConvertOSError(ioctl(os_handle, SIOCGIFNETMASK, &ifr)))
    return FALSE;

  net_mask = sin->sin_addr;
  return TRUE;
#else
  return FALSE;
#endif //!__BEOS__
}


BOOL PEthSocket::GetFilter(unsigned & mask, WORD & type)
{
  if (!IsOpen())
    return FALSE;

#ifndef __BEOS__
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
#else
  return FALSE;
#endif //!__BEOS__
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

#ifndef __BEOS__
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
#else
  return FALSE;
#endif //!__BEOS__
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
    if (len <= sizeof(macHeader)) {
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
  strcpy(to.sa_data, channelName);
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


BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
#if defined(P_LINUX)

  PTextFile procfile;
  if (!procfile.Open("/proc/net/route", PFile::ReadOnly))
    return FALSE;

  for (;;) {
    // Ignore heading line or remainder of route line
    procfile.ignore(1000, '\n');
    if (procfile.eof())
      return TRUE;

    char iface[20];
    long net_addr, dest_addr, net_mask;
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

#else

#pragma message("Platform requires implemetation of GetRouteTable()")
  return FALSE;

#endif
}


#include "../common/pethsock.cxx"


///////////////////////////////////////////////////////////////////////////////

