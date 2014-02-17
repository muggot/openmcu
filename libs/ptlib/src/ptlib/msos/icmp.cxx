/*
 * icmp.cxx
 *
 * ICMP class implementation for Win32.
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
 * $Log: icmp.cxx,v $
 * Revision 1.16  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.15  2004/02/15 02:53:32  rjongbloed
 * Added compatibility with Windows Mobile 2003, thanks Joerg Schoemer
 *
 * Revision 1.14  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.13  1999/08/08 09:29:37  robertj
 * Changed Success to PingSuccess to avoid namespace collision with X define of the same name
 *
 * Revision 1.12  1999/02/16 08:08:06  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.11  1998/11/30 04:48:39  robertj
 * New directory structure
 *
 * Revision 1.10  1998/09/24 03:30:46  robertj
 * Added open software license.
 *
 * Revision 1.9  1998/01/26 00:53:33  robertj
 * Added error codes, TTL and data buffer to Ping.
 *
 * Revision 1.8  1997/10/03 13:32:46  robertj
 * Changed to late binding so do not need icmp.lib to compile system.
 *
 * Revision 1.7  1996/10/29 13:27:17  robertj
 * Change ICMP to use DLL rather than Winsock
 *
 */

#include <ptlib.h>
#include <ptlib/sockets.h>



#ifdef _WIN32_WCE

#include "Icmpapi.h"

typedef ICMP_ECHO_REPLY         ICMPECHO;
typedef ip_option_information   IPINFO;

#define RTTime RoundTripTime

class PICMPDLL : public PObject
{
  PCLASSINFO(PICMPDLL, PObject);

public:

  HANDLE IcmpCreateFile() {
    return ::IcmpCreateFile();
  }

  BOOL IcmpCloseHandle(HANDLE handle) {
    return ::IcmpCloseHandle(handle);
  }

  DWORD IcmpSendEcho(
    HANDLE   handle,           /* handle returned from IcmpCreateFile() */
    u_long   destAddr,         /* destination IP address (in network order)
/
    void   * sendBuffer,       /* pointer to buffer to send */
    WORD     sendLength,       /* length of data in buffer */
    IPINFO * requestOptions,   /* see structure definition above */
    void   * replyBuffer,      /* structure definitionm above */
    DWORD    replySize,        /* size of reply buffer */
    DWORD    timeout           /* time in milliseconds to wait for reply */
    ) {
    return ::IcmpSendEcho(
      handle,
      destAddr,
      sendBuffer,
      sendLength,
      requestOptions,
      replyBuffer,
      replySize,
      timeout);
  }

  bool IsLoaded() {
    return true;
  }
} ICMP;

#else // _WIN32_WCE

///////////////////////////////////////////////////////////////
//
// Definitions for Microsft ICMP library
//

// return values from IcmpSendEcho

#define IP_STATUS_BASE        11000
#define IP_SUCCESS          0
#define IP_BUF_TOO_SMALL      (IP_STATUS_BASE + 1)
#define IP_DEST_NET_UNREACHABLE    (IP_STATUS_BASE + 2)
#define IP_DEST_HOST_UNREACHABLE  (IP_STATUS_BASE + 3)
#define IP_DEST_PROT_UNREACHABLE  (IP_STATUS_BASE + 4)
#define IP_DEST_PORT_UNREACHABLE  (IP_STATUS_BASE + 5)
#define IP_NO_RESOURCES        (IP_STATUS_BASE + 6)
#define IP_BAD_OPTION        (IP_STATUS_BASE + 7)
#define IP_HW_ERROR          (IP_STATUS_BASE + 8)
#define IP_PACKET_TOO_BIG      (IP_STATUS_BASE + 9)
#define IP_REQ_TIMED_OUT      (IP_STATUS_BASE + 10)
#define IP_BAD_REQ          (IP_STATUS_BASE + 11)
#define IP_BAD_ROUTE        (IP_STATUS_BASE + 12)
#define IP_TTL_EXPIRED_TRANSIT    (IP_STATUS_BASE + 13)
#define IP_TTL_EXPIRED_REASSEM    (IP_STATUS_BASE + 14)
#define IP_PARAM_PROBLEM      (IP_STATUS_BASE + 15)
#define IP_SOURCE_QUENCH      (IP_STATUS_BASE + 16)
#define IP_OPTION_TOO_BIG      (IP_STATUS_BASE + 17)
#define IP_BAD_DESTINATION      (IP_STATUS_BASE + 18)
#define IP_ADDR_DELETED        (IP_STATUS_BASE + 19)
#define IP_SPEC_MTU_CHANGE      (IP_STATUS_BASE + 20)
#define IP_MTU_CHANGE        (IP_STATUS_BASE + 21)
#define IP_UNLOAD          (IP_STATUS_BASE + 22)
#define IP_GENERAL_FAILURE      (IP_STATUS_BASE + 50)
#define MAX_IP_STATUS        IP_GENERAL_FAILURE
#define IP_PENDING          (IP_STATUS_BASE + 255)


// ICMP request options structure

typedef struct ip_info {
     u_char Ttl;               /* Time To Live (used for traceroute) */
     u_char Tos;               /* Type Of Service (usually 0) */
     u_char Flags;             /* IP header flags (usually 0) */
     u_char OptionsSize;       /* Size of options data (usually 0, max 40) */
     u_char FAR *OptionsData;  /* Options data buffer */
} IPINFO;

//
// ICMP reply data
//
// The reply buffer will have an array of ICMP_ECHO_REPLY
// structures, followed by options and the data in ICMP echo reply
// datagram received. You must have room for at least one ICMP
// echo reply structure, plus 8 bytes for an ICMP header.
// 

typedef struct icmp_echo_reply {
     u_long Address;         /* source address */
     u_long Status;          /* IP status value (see below) */
     u_long RTTime;          /* Round Trip Time in milliseconds */
     u_short DataSize;       /* reply data size */
     u_short Reserved;    
     void FAR *Data;         /* reply data buffer */
     struct ip_info Options; /* reply options */
} ICMPECHO;


class PICMPDLL : public PDynaLink
{
  PCLASSINFO(PICMPDLL, PDynaLink)
  public:
    PICMPDLL()
      : PDynaLink("ICMP.DLL")
    {
      if (!GetFunction("IcmpCreateFile", (Function &)IcmpCreateFile) ||
          !GetFunction("IcmpCloseHandle", (Function &)IcmpCloseHandle) ||
          !GetFunction("IcmpSendEcho", (Function &)IcmpSendEcho))
        Close();
    }

    // create an ICMP "handle"
    // returns INVALID_HANDLE_VALUE on error 
    HANDLE (WINAPI *IcmpCreateFile)(void);

    // close a handle allocated by IcmpCreateFile
    // returns FALSE on error
    BOOL (PASCAL *IcmpCloseHandle)(HANDLE handle);

    // Send the ICMP echo command for a "ping"
    DWORD (PASCAL *IcmpSendEcho)(
    HANDLE   handle,           /* handle returned from IcmpCreateFile() */
    u_long   destAddr,         /* destination IP address (in network order) */
    void   * sendBuffer,       /* pointer to buffer to send */
    WORD     sendLength,       /* length of data in buffer */
    IPINFO * requestOptions,   /* see structure definition above */
    void   * replyBuffer,      /* structure definitionm above */
    DWORD    replySize,        /* size of reply buffer */
    DWORD    timeout           /* time in milliseconds to wait for reply */
  );
} ICMP;


#endif // _WIN32_WCE


PICMPSocket::PICMPSocket()
{
  OpenSocket();
}

BOOL PICMPSocket::IsOpen() const
{
  return icmpHandle != NULL;
}


BOOL PICMPSocket::OpenSocket()
{
  return ICMP.IsLoaded() && (icmpHandle = ICMP.IcmpCreateFile()) != NULL;
}


BOOL PICMPSocket::Close()
{
  if (icmpHandle == NULL) 
    return TRUE;

  PAssert(ICMP.IsLoaded(), PLogicError);
  return ICMP.IcmpCloseHandle(icmpHandle);
}

const char * PICMPSocket::GetProtocolName() const
{
  return "icmp";
}

BOOL PICMPSocket::Ping(const PString & host)
{
  PingInfo info;
  return Ping(host, info);
}


BOOL PICMPSocket::Ping(const PString & host, PingInfo & info)
{
  if (!ICMP.IsLoaded())
    return SetErrorValues(NotOpen, EBADF);

  // find address of the host
  PIPSocket::Address addr;
  if (!GetHostAddress(host, addr))
    return SetErrorValues(BadParameter, EINVAL);

  IPINFO requestOptions;
  requestOptions.Ttl = info.ttl;     /* Time To Live (used for traceroute) */
  requestOptions.Tos = 0;            /* Type Of Service (usually 0) */
  requestOptions.Flags = 0;          /* IP header flags (usually 0) */
  requestOptions.OptionsSize = 0;    /* Size of options data (usually 0, max 40) */
  requestOptions.OptionsData = NULL; /* Options data buffer */

  BYTE sendBuffer[32];
  void * sendBufferPtr;
  WORD sendBufferSize;

  if (info.buffer != NULL) {
    sendBufferPtr = (void *)info.buffer;
    PAssert(info.bufferSize < 65535, PInvalidParameter);
    sendBufferSize = (WORD)info.bufferSize;
  }
  else {
    sendBufferPtr = sendBuffer;
    sendBufferSize = sizeof(sendBuffer);
  }

  ICMPECHO * reply = (ICMPECHO *)malloc(sizeof(ICMPECHO)+sendBufferSize);

  if (ICMP.IcmpSendEcho(icmpHandle,
            addr,
            sendBufferPtr, sendBufferSize,
            &requestOptions,
            reply, sizeof(ICMPECHO)+sendBufferSize,
            GetReadTimeout().GetInterval()) != 0) {
    info.delay.SetInterval(reply->RTTime);
    info.remoteAddr = Address((in_addr&)reply->Address);
  }

  GetHostAddress(info.localAddr);

  switch (reply->Status) {
    case IP_SUCCESS :
      info.status = PingSuccess;
      break;

    case IP_DEST_NET_UNREACHABLE :
      info.status = NetworkUnreachable;
      break;

    case IP_DEST_HOST_UNREACHABLE :
      info.status = HostUnreachable;
      break;

    case IP_PACKET_TOO_BIG :
      info.status = PacketTooBig;
      break;

    case IP_REQ_TIMED_OUT :
      info.status = RequestTimedOut;
      break;

    case IP_BAD_ROUTE :
      info.status = BadRoute;
      break;

    case IP_TTL_EXPIRED_TRANSIT :
      info.status = TtlExpiredTransmit;
      break;

    case IP_TTL_EXPIRED_REASSEM :
      info.status = TtlExpiredReassembly;
      break;

    case IP_SOURCE_QUENCH :
      info.status = SourceQuench;
      break;

    case IP_MTU_CHANGE :
      info.status = MtuChange;
      break;

    default :
      info.status = GeneralError;
  }

  free(reply);

  return info.status == PingSuccess;
}


PICMPSocket::PingInfo::PingInfo(WORD id)
{
  identifier = id;
  sequenceNum = 0;
  ttl = 255;
  buffer = NULL;
  status = PingSuccess;
}


// End Of File ///////////////////////////////////////////////////////////////
