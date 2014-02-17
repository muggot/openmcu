/*
 * uicmp.cxx
 *
 * ICMP socket class implementation.
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
 * $Log: uicmp.cxx,v $
 * Revision 1.16  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.15  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.14  2001/09/10 03:03:36  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.13  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.12  2001/03/07 07:00:17  yurik
 * #ifdef'd setsockopt IPPROTO_IP for BeOS
 *
 * Revision 1.11  2001/03/06 22:20:21  craigs
 * Fixed TTL and other stuff so that traceroute is almost possible!
 *
 * Revision 1.10  1999/08/09 04:06:39  robertj
 * Change to avoid name space problem with X windows library
 *
 * Revision 1.9  1999/02/22 13:26:54  robertj
 * BeOS port changes.
 *
 * Revision 1.8  1998/11/30 21:52:06  robertj
 * New directory structure.
 *
 * Revision 1.7  1998/09/24 04:12:29  robertj
 * Added open software license.
 *
 * Revision 1.6  1998/08/26 01:45:56  craigs
 * Fixed error in IPHdr
 *
 * Revision 1.5  1998/01/26 07:27:09  robertj
 * Added part support for extra ping info. Still needs TTL for traceroute.
 *
 * Revision 1.4  1996/11/16 11:12:56  craigs
 * Fixed problem with work misaligns under SOlaris
 *
 * Revision 1.3  1996/10/31 10:20:07  craigs
 * Moved ICMP implementation into here, as it is now platform dependent
 *
 * Revision 1.6  1996/09/14 13:09:34  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.5  1996/08/11 06:52:14  robertj
 * Oops
 *
 * Revision 1.4  1996/08/07 13:40:57  robertj
 * Fixed sparc memory alignment problem from int 64
 *
 * Revision 1.3  1996/06/03 10:03:10  robertj
 * Changed ping to return more parameters.
 *
 * Revision 1.2  1996/05/30 10:08:51  robertj
 * Fixed bug in ping (checksum incorrect).
 *
 * Revision 1.1  1996/05/15 21:11:35  robertj
 * Initial revision
 *
 */

#pragma implementation "icmpsock.h"

#include <ptlib.h>
#include <ptlib/sockets.h>

#define  MAX_IP_LEN  60
#define  MAX_ICMP_LEN  76
#define  ICMP_DATA_LEN  (64-8)
#define  RX_BUFFER_SIZE  (MAX_IP_LEN+MAX_ICMP_LEN+ICMP_DATA_LEN)

#define  ICMP_ECHO_REPLY  0
#define  ICMP_ECHO  8

#define  ICMP_TIMXCEED  11


typedef struct {
  BYTE   type;
  BYTE   code;
  WORD   checksum;

  WORD   id;
  WORD   sequence;

  PInt64 sendtime;
  BYTE   data[ICMP_DATA_LEN-sizeof(PInt64)];
} ICMPPacket;


typedef struct {
  BYTE verIhl;
  BYTE typeOfService;
  WORD totalLength;
  WORD identification;
  WORD fragOff;
  BYTE timeToLive;
  BYTE protocol;
  WORD checksum;
  BYTE sourceAddr[4];
  BYTE destAddr[4];
} IPHdr;


static WORD CalcChecksum(void * p, PINDEX len)
{
  WORD * ptr = (WORD *)p;
  DWORD sum = 0;
  while (len > 1) {
    sum += *ptr++;
    len-=2;
  }

  if (len > 0) {
    WORD t = *(BYTE *)ptr;
    sum += t;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (WORD)~sum;
}


PICMPSocket::PICMPSocket()
{
  OpenSocket();
}


BOOL PICMPSocket::Ping(const PString & host)
{
  PingInfo info;
  return Ping(host, info);
}


BOOL PICMPSocket::Ping(const PString & host, PingInfo & info)
{
  if (!WritePing(host, info))
    return FALSE;

  return ReadPing(info);
}


BOOL PICMPSocket::WritePing(const PString & host, PingInfo & info)
{
  // find address of the host
  PIPSocket::Address addr;
  if (!GetHostAddress(host, addr))
    return SetErrorValues(BadParameter, EINVAL);

  // create the ICMP packet
  ICMPPacket packet;

  // clear the packet including data area
  memset(&packet, 0, sizeof(packet));

  packet.type       = ICMP_ECHO;
  packet.sequence   = info.sequenceNum;
  packet.id         = info.identifier;

#ifndef BE_BONELESS
  if (info.ttl != 0) {
    char ttl = (char)info.ttl;
    if (::setsockopt(os_handle, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0)
      return FALSE;
  }
#endif

  // set the send time
  packet.sendtime = PTimer::Tick().GetMilliSeconds();

  // calculate the checksum
  packet.checksum = CalcChecksum(&packet, sizeof(packet));

  // send the packet
  return WriteTo(&packet, sizeof(packet), addr, 0);
}


BOOL PICMPSocket::ReadPing(PingInfo & info)
{
  // receive a packet
  BYTE packet[RX_BUFFER_SIZE];
  IPHdr      * ipHdr;
  ICMPPacket * icmpPacket;
  WORD port;
  PInt64 now;
  PTimer timeout(GetReadTimeout());

  for (;;) {
    memset(&packet, 0, sizeof(packet));

    if (!ReadFrom(packet, sizeof(packet), info.remoteAddr, port))
      return FALSE;

    now  = PTimer::Tick().GetMilliSeconds();
    ipHdr      = (IPHdr *)packet;
    icmpPacket = (ICMPPacket *)(packet + ((ipHdr->verIhl & 0xf) << 2));

    if ((      icmpPacket->type == ICMP_ECHO_REPLY) && 
        ((WORD)icmpPacket->id   == info.identifier)) {
      info.status = PingSuccess;
      break;
    }

    if (icmpPacket->type == ICMP_TIMXCEED) {
      info.status = TtlExpiredTransmit;
      break;
    }

    if (!timeout.IsRunning())
      return FALSE;
  }

  info.remoteAddr = Address(ipHdr->sourceAddr[0], ipHdr->sourceAddr[1],
                            ipHdr->sourceAddr[2], ipHdr->sourceAddr[3]);
  info.localAddr  = Address(ipHdr->destAddr[0], ipHdr->destAddr[1],
                            ipHdr->destAddr[2], ipHdr->destAddr[3]);

  // calc round trip time. Be careful, as unaligned "long long" ints
  // can cause problems on some platforms
#if defined(P_SUN4) || defined(P_SOLARIS)
  PInt64 then;
  BYTE * pthen = (BYTE *)&then;
  BYTE * psendtime = (BYTE *)&icmpPacket->sendtime;
  memcpy(pthen, psendtime, sizeof(PInt64));
  info.delay.SetInterval(now - then);
#else
  info.delay.SetInterval(now - icmpPacket->sendtime);
#endif

  info.sequenceNum = icmpPacket->sequence;

  return TRUE;
}


BOOL PICMPSocket::OpenSocket()
{
#if !defined BE_BONELESS && !defined(P_VXWORKS)
  struct protoent * p = ::getprotobyname(GetProtocolName());
  if (p == NULL)
    return ConvertOSError(-1);
  return ConvertOSError(os_handle = os_socket(AF_INET, SOCK_RAW, p->p_proto));
#else  // Raw sockets not supported in BeOS R4 or VxWorks.
  return ConvertOSError(os_handle = os_socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP));
#endif //!defined BE_BONELESS && !defined(P_VXWORKS)
}


const char * PICMPSocket::GetProtocolName() const
{
  return "icmp";
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
